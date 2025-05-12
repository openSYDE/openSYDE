//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for system node data pool list import report (implementation)

   Dialog for system node data pool list import report.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
//Cannot use precomp headers because of pc lint?
//#include "precomp_headers.hpp"

#include <QFileInfo>

#include "C_SdUtil.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_ImpUtil.hpp"
#include "C_Uti.hpp"
#include "C_CieUtil.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_CieImportDatapoolSelectWidget.hpp"
#include "C_CieImportReportWidget.hpp"
#include "ui_C_CieImportReportWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_CieImportReportWidget::hc_HTML_TABLE_HEADER_START =
   "<td align=\"left\" valign=\"top\" style=\"padding: 5px 18px 5px 0px;white-space:pre;font-weight:bold;\">";
const QString C_CieImportReportWidget::hc_HTML_TABLE_DATA_START =
   "<td align=\"left\" valign=\"top\" style=\"padding: 5px 18px 5px 0px;white-space:pre;\">";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent                      Reference to parent
   \param[in]      orc_FilePath                    Loaded file path
   \param[in]      ou32_BusIndex                   Bus index
   \param[in]      oe_ProtocolType                 Type of comm protocol
   \param[in]      orc_ImportDataAssigned          Import data assigned
   \param[in]      orc_SkippedImportDataAssigned   Skipped import data assigned
   \param[in]      opc_NodeNameReplacement         Node name replacement
   \param[in]      oq_IsCanOpen                    Is CANopen
   \param[in]      oq_UniqueAddRequested           Flag to add messages unique
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieImportReportWidget::C_CieImportReportWidget(C_OgePopUpDialog & orc_Parent, const QString & orc_FilePath,
                                                 const uint32_t ou32_BusIndex,
                                                 const C_OscCanProtocol::E_Type oe_ProtocolType,
                                                 const std::vector<C_CieImportDataAssignment> & orc_ImportDataAssigned,
                                                 const std::vector<C_CieImportDataAssignment> & orc_SkippedImportDataAssigned, const stw::scl::C_SclString * const opc_NodeNameReplacement, const bool oq_IsCanOpen,
                                                 const bool oq_UniqueAddRequested) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_CieImportReportWidget),
   mrc_ParentDialog(orc_Parent),
   mc_FilePath(orc_FilePath),
   mu32_BusIndex(ou32_BusIndex),
   me_ProtocolType(oe_ProtocolType),
   mc_ImportedAssignedData(orc_ImportDataAssigned),
   mc_SkippedImportedAssignedData(orc_SkippedImportDataAssigned),
   mpc_NodeNameReplacement(opc_NodeNameReplacement),
   mq_UniqueAddRequested(oq_UniqueAddRequested)
{
   const QFileInfo c_FileInfo(orc_FilePath);
   const bool q_IsEdsOrDcfImport = C_CieImportReportWidget::mh_IsEdsOrDcfImport(c_FileInfo.completeSuffix().toUpper());

   mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // set main title
   this->mrc_ParentDialog.SetTitle(static_cast<QString>(C_GtGetText::h_GetText("%1 File Import")).arg(c_FileInfo.
                                                                                                      completeSuffix().
                                                                                                      toUpper()));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Report"));

   //Trigger report
   C_CieUtil::h_AdaptImportMessages(this->mc_ImportedAssignedData, this->me_ProtocolType, q_IsEdsOrDcfImport);
   C_CieUtil::h_AdaptImportMessages(this->mc_SkippedImportedAssignedData, this->me_ProtocolType, q_IsEdsOrDcfImport);
   tgl_assert(m_ShowReport(c_FileInfo.
                           completeSuffix().
                           toUpper(), oq_IsCanOpen) == C_NO_ERR);

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this, &C_CieImportReportWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked, this, &C_CieImportReportWidget::m_CancelClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieImportReportWidget::~C_CieImportReportWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportReportWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelHeadingReport->setText(C_GtGetText::h_GetText("Details"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("Import"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get initial report part

   \param[in]  orc_ImportDataAssignment      Import data assignment
   \param[in]  orc_ImportedFilePath          Imported file path
   \param[in]  oq_IsCanOpenCase              Is can open case
   \param[in]  oq_AddMessageAdaptationInfo   Add message adaptation info

   \return
   Initial report part
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CieImportReportWidget::h_GetInitialReportPart(
   const std::vector<C_CieImportDataAssignment> & orc_ImportDataAssignment, const QString & orc_ImportedFilePath,
   const bool oq_IsCanOpenCase, const bool oq_AddMessageAdaptationInfo)
{
   const QFileInfo c_FileInfo(orc_ImportedFilePath);
   QString c_CompleteLog;
   QString c_ReadContent;

   //Sum up read content
   if (oq_IsCanOpenCase == true)
   {
      c_ReadContent += C_GtGetText::h_GetText("PDOs: ");
   }
   else
   {
      c_ReadContent += C_GtGetText::h_GetText("Nodes: ");
   }
   for (uint32_t u32_ItNodes = 0; u32_ItNodes < orc_ImportDataAssignment.size(); u32_ItNodes++)
   {
      const C_CieImportDataAssignment & rc_CurData = orc_ImportDataAssignment[u32_ItNodes];
      c_ReadContent += rc_CurData.c_ImportData.c_NodeName;
      c_ReadContent += static_cast<QString>(C_GtGetText::h_GetText(" (%1 Tx / %2 Rx); ")).
                       arg(rc_CurData.c_ImportData.c_Core.c_OscTxMessageData.size()).
                       arg(rc_CurData.c_ImportData.c_Core.c_OscRxMessageData.size());
   }
   c_ReadContent += "<br/>";
   c_ReadContent += C_GtGetText::h_GetText("For parsing errors, warnings and detailed information see ");
   c_ReadContent += C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"), mc_STYLE_GUIDE_COLOR_LINK,
                                     C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str());
   c_ReadContent += ".";

   //Start
   c_CompleteLog += "<html>";
   c_CompleteLog += "<body>";

   //Source File Info
   c_CompleteLog += C_ImpUtil::h_FormatSourceFileInfoForReport(orc_ImportedFilePath, c_ReadContent);

   //Update log file
   C_OscLoggingHandler::h_Flush();

   //Info
   if (oq_AddMessageAdaptationInfo)
   {
      c_CompleteLog += "<h3>";
      c_CompleteLog += C_GtGetText::h_GetText("Import Preview");
      c_CompleteLog += "</h3>";
      c_CompleteLog += "<p>";
      c_CompleteLog += C_GtGetText::h_GetText("Info: Message and signal names are adapted as follows:");
      c_CompleteLog += "<br/>";
      c_CompleteLog += "- ";
      c_CompleteLog += C_GtGetText::h_GetText("Eliminate spaces");
      c_CompleteLog += "<br/>";
      c_CompleteLog += "- ";
      c_CompleteLog += static_cast<QString>(C_GtGetText::h_GetText("Cut to %1 (= project setting) characters")).arg(
         C_PuiSdHandler::h_GetInstance()->GetNameMaxCharLimit());
      c_CompleteLog += "</p>";
      if ((oq_IsCanOpenCase == true) && (c_FileInfo.completeSuffix().toLower() == "dcf"))
      {
         c_CompleteLog += "<p>";
         c_CompleteLog += C_GtGetText::h_GetText("Node ID and device name will not be adopted from the DCF - File.");
         c_CompleteLog += "</p>";
      }
   }
   return c_CompleteLog;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get message table content

   \param[in,out]  orc_ImportTable           Import table
   \param[in]      orc_Suffix                Suffix
   \param[in,out]  orc_ImportDataAssignment  Import data assignment
   \param[in]      oe_ProtocolType           Protocol type
   \param[in]      opc_NodeNameReplacement   Node name replacement
   \param[in]      orc_Heading               Heading
   \param[in]      oq_ReplaceMessageNames    Replace message names

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieImportReportWidget::h_GetMessageTableContent(QString & orc_ImportTable, const QString & orc_Suffix,
                                                          std::vector<C_CieImportDataAssignment> & orc_ImportDataAssignment, const C_OscCanProtocol::E_Type oe_ProtocolType, const stw::scl::C_SclString * const opc_NodeNameReplacement, const QString & orc_Heading,
                                                          const bool oq_ReplaceMessageNames)
{
   int32_t s32_Retval = C_NO_ERR;
   uint32_t u32_MessageCount = 0UL;
   QString c_RawTable;

   mh_GetTableWithMessageEntries(s32_Retval, c_RawTable, u32_MessageCount, orc_Suffix,
                                 orc_ImportDataAssignment, oe_ProtocolType, opc_NodeNameReplacement,
                                 oq_ReplaceMessageNames);
   if ((s32_Retval == C_NO_ERR) && (c_RawTable.isEmpty() == false))
   {
      //New table
      orc_ImportTable += "<p><b>";
      //Translation: 1=Heading 2=Number of messages
      orc_ImportTable += static_cast<QString>(C_GtGetText::h_GetText("%1 (%2)")).arg(orc_Heading).arg(
         u32_MessageCount);
      orc_ImportTable += "</b></p>";
      orc_ImportTable += "<table>";
      //Header
      orc_ImportTable += "<tr>";
      orc_ImportTable += C_CieImportReportWidget::hc_HTML_TABLE_HEADER_START;
      orc_ImportTable += "#</td>";
      orc_ImportTable += C_CieImportReportWidget::hc_HTML_TABLE_HEADER_START;
      orc_ImportTable += C_GtGetText::h_GetText("Name");
      orc_ImportTable += "</td>";
      orc_ImportTable += C_CieImportReportWidget::hc_HTML_TABLE_HEADER_START;
      orc_ImportTable += C_GtGetText::h_GetText("Signals");
      orc_ImportTable += "</td>";
      orc_ImportTable += C_CieImportReportWidget::hc_HTML_TABLE_HEADER_START;
      orc_ImportTable += C_GtGetText::h_GetText("Warnings");
      orc_ImportTable += "</td>";
      orc_ImportTable += "</tr>";
      //Content
      orc_ImportTable += c_RawTable;
      orc_ImportTable += "</table>";
   }
   else
   {
      orc_ImportTable = "";
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportReportWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
         this->m_OkClicked();
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
void C_CieImportReportWidget::m_OkClicked(void)
{
   bool q_Continue = true;

   for (uint32_t u32_ItNodes = 0; u32_ItNodes < this->mc_ImportedAssignedData.size(); u32_ItNodes++)
   {
      C_CieImportDataAssignment & rc_CurData = this->mc_ImportedAssignedData[u32_ItNodes];
      uint32_t u32_NumberCommDps =
         C_PuiSdHandler::h_GetInstance()->GetOscCanDataPools(rc_CurData.u32_OsyNodeIndex, this->me_ProtocolType).size();

      // reset
      rc_CurData.s32_DatapoolIndexForNew = -1;

      // we need a datapool index if there exist new messages
      if (rc_CurData.u32_NewMessageCount > 0)
      {
         // first of all: if no COMM Datapool of this protocol exists add one
         if (u32_NumberCommDps == 0)
         {
            // No COM datapool yet. New one must be created if permissible
            if (C_SdUtil::h_CheckDatapoolNumber(rc_CurData.u32_OsyNodeIndex, false, this) == true)
            {
               tgl_assert(C_PuiSdHandler::h_GetInstance()->AddAutoGenCommDataPool(rc_CurData.u32_OsyNodeIndex,
                                                                                  this->me_ProtocolType) == C_NO_ERR);
               u32_NumberCommDps++; // we just added one!
            }
            else
            {
               q_Continue = false;
            }
         }

         // if there is exactly one datapool of this protocol type get its index
         if (q_Continue == true)
         {
            if (u32_NumberCommDps == 1)
            {
               const C_OscNode * const pc_Node =
                  C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(rc_CurData.u32_OsyNodeIndex);

               // get datapool index of the corresponding datapool
               if (pc_Node != NULL)
               {
                  for (uint32_t u32_ItDatapool = 0; u32_ItDatapool < pc_Node->c_DataPools.size(); ++u32_ItDatapool)
                  {
                     if (pc_Node->c_DataPools[u32_ItDatapool].e_Type == C_OscNodeDataPool::eCOM)
                     {
                        if (C_PuiSdUtil::h_GetRelatedCanProtocolType(rc_CurData.u32_OsyNodeIndex,
                                                                     u32_ItDatapool) == this->me_ProtocolType)
                        {
                           rc_CurData.s32_DatapoolIndexForNew = static_cast<int32_t>(u32_ItDatapool);
                           break;
                        }
                     }
                  }
               }
            }
            else
            {
               // there are multiple COMM Datapools of this protocol:
               // ask user which COMM Datapool the new messages should get inserted at

               const QPointer<C_OgePopUpDialog> c_Popup = new C_OgePopUpDialog(this, this);
               C_CieImportDatapoolSelectWidget * const pc_DialogImportReport =
                  new C_CieImportDatapoolSelectWidget(*c_Popup, rc_CurData.u32_OsyNodeIndex, this->me_ProtocolType);

               // resize
               c_Popup->SetSize(QSize(700, 400));

               // execute datapool popup
               if (c_Popup->exec() == static_cast<int32_t>(QDialog::Accepted))
               {
                  rc_CurData.s32_DatapoolIndexForNew = pc_DialogImportReport->GetSelectedDatapoolIndex();
               }
               else
               {
                  q_Continue = false;
               }

               if (c_Popup != NULL)
               {
                  c_Popup->HideOverlay();
                  c_Popup->deleteLater();
               }
            } //lint !e429  //no memory leak because of the parent of pc_DialogImportReport and the Qt memory
         }
      }
      else
      {
         // index not needed but should be valid
         rc_CurData.s32_DatapoolIndexForNew = 0;
      }
   }

   if (q_Continue == true)
   {
      // only accept if user did not cancel datapool selection (else caller does not know that user canceled)
      this->mrc_ParentDialog.accept();
      m_InsertMessages();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportReportWidget::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show/update report

   \param[in]  orc_Suffix     Suffix of the message file
   \param[in]  oq_IsCanOpen   Is CANopen

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieImportReportWidget::m_ShowReport(const QString & orc_Suffix, const bool oq_IsCanOpen)
{
   int32_t s32_Retval;
   QString c_ImportTable = "";

   int32_t s32_SkippedRetval;
   QString c_SkippedTable = "";

   // Check
   s32_Retval = m_CheckMessages(this->mc_ImportedAssignedData);
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = m_CheckMessageMatches(orc_Suffix, this->mc_ImportedAssignedData);
   }
   // Get table with message entries
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_CieImportReportWidget::h_GetMessageTableContent(c_ImportTable, orc_Suffix,
                                                                     this->mc_ImportedAssignedData,
                                                                     this->me_ProtocolType,
                                                                     this->mpc_NodeNameReplacement,
                                                                     C_GtGetText::h_GetText("Import Messages"), true);
   }

   // Check skipped
   s32_SkippedRetval = m_CheckMessages(this->mc_SkippedImportedAssignedData);
   if (s32_SkippedRetval == C_NO_ERR)
   {
      s32_SkippedRetval = m_CheckMessageMatches(orc_Suffix, this->mc_SkippedImportedAssignedData);
   }
   // Get skipped table with message entries
   if (s32_SkippedRetval == C_NO_ERR)
   {
      s32_SkippedRetval = C_CieImportReportWidget::h_GetMessageTableContent(c_SkippedTable, orc_Suffix,
                                                                            this->mc_SkippedImportedAssignedData,
                                                                            this->me_ProtocolType,
                                                                            this->mpc_NodeNameReplacement,
                                                                            C_GtGetText::h_GetText(
                                                                               "Skipped Messages"), true);
   }

   if (s32_Retval == C_NO_ERR)
   {
      QString c_CompleteLog = C_CieImportReportWidget::h_GetInitialReportPart(this->mc_ImportedAssignedData,
                                                                              this->mc_FilePath,
                                                                              oq_IsCanOpen,
                                                                              c_ImportTable.isEmpty() == false);
      //Content
      c_CompleteLog += c_ImportTable;
      //Content
      c_CompleteLog += c_SkippedTable;

      //End
      c_CompleteLog += "</body>";
      c_CompleteLog += "</html>";
      this->mpc_Ui->pc_TextBrowserReport->setHtml(c_CompleteLog);
   }

   if (s32_Retval != C_NO_ERR)
   {
      s32_Retval = C_CONFIG;
   }

   if (s32_SkippedRetval != C_NO_ERR)
   {
      osc_write_log_warning("Skipped Messages while importing Messages", "Configuration Problem");
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check messages

   \param[in,out]  orc_ImportDataAssignment  Import data assignment

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieImportReportWidget::m_CheckMessages(const std::vector<C_CieImportDataAssignment> & orc_ImportDataAssignment)
const
{
   int32_t s32_Retval = C_NO_ERR;

   //Each imported node
   for (uint32_t u32_ItNodes = 0; (u32_ItNodes < orc_ImportDataAssignment.size()) && (s32_Retval == C_NO_ERR);
        u32_ItNodes++)
   {
      const C_CieImportDataAssignment & rc_CurData = orc_ImportDataAssignment[u32_ItNodes];

      if (rc_CurData.c_ImportData.c_Core.c_OscRxMessageData.size() !=
          rc_CurData.c_ImportData.c_Ui.c_UiRxMessageData.size())
      {
         s32_Retval = C_CONFIG;
      }
      else
      {
         uint32_t u32_SignalSum = 0;
         for (uint32_t u32_ItMessage = 0;
              (u32_ItMessage < rc_CurData.c_ImportData.c_Core.c_OscRxMessageData.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItMessage)
         {
            const C_OscCanMessage & rc_OscCurMessage = rc_CurData.c_ImportData.c_Core.c_OscRxMessageData[u32_ItMessage];
            const C_PuiSdNodeCanMessage & rc_UiCurMessage =
               rc_CurData.c_ImportData.c_Ui.c_UiRxMessageData[u32_ItMessage];
            if (rc_OscCurMessage.c_Signals.size() != rc_UiCurMessage.c_Signals.size())
            {
               s32_Retval = C_CONFIG;
            }
            for (uint32_t u32_ItSignal = 0;
                 (u32_ItSignal < rc_OscCurMessage.c_Signals.size()) && (s32_Retval == C_NO_ERR); ++u32_ItSignal)
            {
               const C_OscCanSignal & rc_CurSignal = rc_OscCurMessage.c_Signals[u32_ItSignal];
               if (rc_CurSignal.u32_ComDataElementIndex >= rc_CurData.c_ImportData.c_Core.c_OscRxSignalData.size())
               {
                  s32_Retval = C_CONFIG;
               }
            }
            u32_SignalSum += rc_OscCurMessage.c_Signals.size();
         }
         if (rc_CurData.c_ImportData.c_Core.c_OscRxSignalData.size() != u32_SignalSum)
         {
            s32_Retval = C_CONFIG;
         }
      }
      if (rc_CurData.c_ImportData.c_Core.c_OscTxMessageData.size() !=
          rc_CurData.c_ImportData.c_Ui.c_UiTxMessageData.size())
      {
         s32_Retval = C_CONFIG;
      }
      else
      {
         uint32_t u32_SignalSum = 0;
         for (uint32_t u32_ItMessage = 0;
              (u32_ItMessage < rc_CurData.c_ImportData.c_Core.c_OscTxMessageData.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItMessage)
         {
            const C_OscCanMessage & rc_OscCurMessage = rc_CurData.c_ImportData.c_Core.c_OscTxMessageData[u32_ItMessage];
            const C_PuiSdNodeCanMessage & rc_UiCurMessage =
               rc_CurData.c_ImportData.c_Ui.c_UiTxMessageData[u32_ItMessage];
            if (rc_OscCurMessage.c_Signals.size() != rc_UiCurMessage.c_Signals.size())
            {
               s32_Retval = C_CONFIG;
            }
            for (uint32_t u32_ItSignal = 0;
                 (u32_ItSignal < rc_OscCurMessage.c_Signals.size()) && (s32_Retval == C_NO_ERR); ++u32_ItSignal)
            {
               const C_OscCanSignal & rc_CurSignal = rc_OscCurMessage.c_Signals[u32_ItSignal];
               if (rc_CurSignal.u32_ComDataElementIndex >= rc_CurData.c_ImportData.c_Core.c_OscTxSignalData.size())
               {
                  s32_Retval = C_CONFIG;
               }
            }
            u32_SignalSum += rc_OscCurMessage.c_Signals.size();
         }
         if (rc_CurData.c_ImportData.c_Core.c_OscTxSignalData.size() != u32_SignalSum)
         {
            s32_Retval = C_CONFIG;
         }
      }
      if (rc_CurData.c_ImportData.c_Core.c_OscRxSignalData.size() !=
          rc_CurData.c_ImportData.c_Ui.c_UiRxSignalData.size())
      {
         s32_Retval = C_CONFIG;
      }
      if (rc_CurData.c_ImportData.c_Core.c_OscTxSignalData.size() !=
          rc_CurData.c_ImportData.c_Ui.c_UiTxSignalData.size())
      {
         s32_Retval = C_CONFIG;
      }
      if (rc_CurData.c_ImportData.c_Core.c_OscRxMessageData.size() !=
          rc_CurData.c_ImportData.c_Core.c_WarningMessagesPerRxMessage.size())
      {
         s32_Retval = C_CONFIG;
      }
      if (rc_CurData.c_ImportData.c_Core.c_OscTxMessageData.size() !=
          rc_CurData.c_ImportData.c_Core.c_WarningMessagesPerTxMessage.size())
      {
         s32_Retval = C_CONFIG;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check for matching messages

   \param[in]      orc_Suffix                Suffix of the message file
   \param[in,out]  orc_ImportDataAssignment  Import data assignment

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieImportReportWidget::m_CheckMessageMatches(const QString & orc_Suffix,
                                                       std::vector<C_CieImportDataAssignment> & orc_ImportDataAssignment)
{
   int32_t s32_Retval = C_NO_ERR;

   //Each imported node
   for (uint32_t u32_ItNodes = 0; (u32_ItNodes < orc_ImportDataAssignment.size()) && (s32_Retval == C_NO_ERR);
        u32_ItNodes++)
   {
      C_CieImportDataAssignment & rc_CurData = orc_ImportDataAssignment[u32_ItNodes];

      // Make sure these are empty
      rc_CurData.c_RxMessageOverrideIndices.clear();
      rc_CurData.c_TxMessageOverrideIndices.clear();

      std::vector<stw::opensyde_core::C_OscCanMessage>::iterator c_ImportMessageIt;
      std::pair<int32_t, int32_t> c_OverrideInfo;
      // Check every Tx message of import file
      uint32_t u32_TxMessageCount = 0;
      uint32_t u32_SrdoMessageCount = 0;
      for (c_ImportMessageIt = rc_CurData.c_ImportData.c_Core.c_OscTxMessageData.begin();
           (c_ImportMessageIt != rc_CurData.c_ImportData.c_Core.c_OscTxMessageData.end()) && (s32_Retval == C_NO_ERR);
           ++c_ImportMessageIt)
      {
         const bool q_IsSrdo = C_CieUtil::h_GetIsSrdoFromMessageIsSrdoVector(
            rc_CurData.c_ImportData.c_Core.c_EdsOscTxMessageIsSrdo, u32_TxMessageCount);
         s32_Retval = m_GetMessageOverrideInfo(*c_ImportMessageIt, true, q_IsSrdo, rc_CurData.u32_OsyNodeIndex,
                                               rc_CurData.u32_OsyInterfaceIndex, c_OverrideInfo, u32_TxMessageCount,
                                               orc_Suffix, u32_SrdoMessageCount);
         rc_CurData.c_TxMessageOverrideIndices.push_back(c_OverrideInfo);
         u32_TxMessageCount++;
      }

      // Check every Rx message of import file
      uint32_t u32_RxMessageCount = 0;
      for (c_ImportMessageIt = rc_CurData.c_ImportData.c_Core.c_OscRxMessageData.begin();
           (c_ImportMessageIt != rc_CurData.c_ImportData.c_Core.c_OscRxMessageData.end()) && (s32_Retval == C_NO_ERR);
           ++c_ImportMessageIt)
      {
         const bool q_IsSrdo = C_CieUtil::h_GetIsSrdoFromMessageIsSrdoVector(
            rc_CurData.c_ImportData.c_Core.c_EdsOscRxMessageIsSrdo, u32_RxMessageCount);
         s32_Retval = m_GetMessageOverrideInfo(*c_ImportMessageIt, false, q_IsSrdo, rc_CurData.u32_OsyNodeIndex,
                                               rc_CurData.u32_OsyInterfaceIndex, c_OverrideInfo, u32_RxMessageCount,
                                               orc_Suffix, u32_SrdoMessageCount);
         rc_CurData.c_RxMessageOverrideIndices.push_back(c_OverrideInfo);
         u32_RxMessageCount++;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Compare a import message with existing messages to find out if the message is new or just an update.

   \param[in]      orc_ImportMessageToFind   message from import file that should be checked for existing message
   \param[in]      oq_Tx                     flag if message is Tx
   \param[in]      oq_IsSrdo                 Flag if message is SRDO
   \param[in]      ou32_OsyNodeIndex         Osy node index
   \param[in]      ou32_OsyInterfaceIndex    Osy interface index
   \param[out]     orc_MessageOverrideInfo   pair of datapool index and index of message in corresponding protocol
   \param[in]      ou32_MessageIndex         Message count index
   \param[in]      orc_Suffix                Suffix of the message file
   \param[in,out]  oru32_SrdoIndex           SRDO index

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieImportReportWidget::m_GetMessageOverrideInfo(C_OscCanMessage & orc_ImportMessageToFind, const bool oq_Tx,
                                                          const bool oq_IsSrdo, const uint32_t ou32_OsyNodeIndex,
                                                          const uint32_t ou32_OsyInterfaceIndex, std::pair<int32_t,
                                                                                                           int32_t> & orc_MessageOverrideInfo, const uint32_t ou32_MessageIndex, const QString & orc_Suffix,
                                                          uint32_t & oru32_SrdoIndex) const
{
   int32_t s32_Retval = C_NO_ERR;

   // reset
   orc_MessageOverrideInfo = std::make_pair(-1, -1);
   if (this->mq_UniqueAddRequested == false)
   {
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_OsyNodeIndex);

      if (pc_Node != NULL)
      {
         if ((orc_Suffix == "EDS") || (orc_Suffix == "DCF"))
         {
            orc_ImportMessageToFind.c_Name = C_CieUtil::h_GetMessageName(*pc_Node, oq_Tx, ou32_MessageIndex,
                                                                         this->me_ProtocolType,
                                                                         this->mpc_NodeNameReplacement,
                                                                         oq_IsSrdo, oru32_SrdoIndex).toStdString().
                                             c_str();
         }

         //Only check other protocol matches
         if (this->me_ProtocolType != C_OscCanProtocol::eCAN_OPEN)
         {
            std::vector<const C_OscCanProtocol *> c_Protocols = pc_Node->GetCanProtocolsConst(this->me_ProtocolType);
            std::vector<const C_OscCanProtocol *>::const_iterator c_ProtocolIt;

            // Iterate Protocols (resp. Datapools)
            for (c_ProtocolIt = c_Protocols.begin(); c_ProtocolIt != c_Protocols.end(); ++c_ProtocolIt)
            {
               const C_OscCanProtocol * const pc_Protocol = *c_ProtocolIt;
               if (pc_Protocol != NULL)
               {
                  if (ou32_OsyInterfaceIndex < pc_Protocol->c_ComMessages.size())
                  {
                     const C_OscCanMessageContainer & rc_MessageContainer =
                        pc_Protocol->c_ComMessages[ou32_OsyInterfaceIndex];
                     const std::vector<C_OscCanMessage> & rc_Messages = rc_MessageContainer.GetMessagesConst(oq_Tx);

                     // Check all existing messages for a match (CAN Id and Name)
                     for (uint32_t u32_MessageIt = 0; u32_MessageIt < rc_Messages.size(); u32_MessageIt++)
                     {
                        const C_OscCanMessage & rc_ExistingMessage = rc_Messages[u32_MessageIt];
                        if (((rc_ExistingMessage.u32_CanId == orc_ImportMessageToFind.u32_CanId) &&
                             (rc_ExistingMessage.q_IsExtended == orc_ImportMessageToFind.q_IsExtended)) &&
                            (rc_ExistingMessage.c_Name == orc_ImportMessageToFind.c_Name))
                        {
                           orc_MessageOverrideInfo =
                              std::make_pair(static_cast<int32_t>(pc_Protocol->u32_DataPoolIndex),
                                             static_cast<int32_t>(u32_MessageIt));
                           break; // no further search necessary if found
                        }
                     }
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
            }
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table with message entries

   \param[out]     ors32_Retval              Retval
   \param[in,out]  orc_ImportTable           Import table
   \param[out]     oru32_MessageCount        Message count
   \param[in]      orc_Suffix                Suffix
   \param[in,out]  orc_ImportDataAssignment  Import data assignment
   \param[in]      oe_ProtocolType           Protocol type
   \param[in]      opc_NodeNameReplacement   Node name replacement
   \param[in]      oq_ReplaceMessageNames    Replace message names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportReportWidget::mh_GetTableWithMessageEntries(int32_t & ors32_Retval, QString & orc_ImportTable,
                                                            uint32_t & oru32_MessageCount, const QString & orc_Suffix,
                                                            std::vector<C_CieImportDataAssignment> & orc_ImportDataAssignment, const C_OscCanProtocol::E_Type oe_ProtocolType, const stw::scl::C_SclString * const opc_NodeNameReplacement,
                                                            const bool oq_ReplaceMessageNames)
{
   std::vector<C_OscCanMessage> c_MessagesList; // for listing every message only once

   //Each imported node
   for (uint32_t u32_ItNodes = 0; (u32_ItNodes < orc_ImportDataAssignment.size()) && (ors32_Retval == C_NO_ERR);
        u32_ItNodes++)
   {
      C_CieImportDataAssignment & rc_CurData = orc_ImportDataAssignment[u32_ItNodes];
      uint32_t u32_NewSrdoMessageCount = 0UL;

      rc_CurData.u32_NewMessageCount = 0UL;

      //Tx
      ors32_Retval = mh_GetMessageEntries(oru32_MessageCount, rc_CurData.u32_NewMessageCount, orc_ImportTable,
                                          c_MessagesList,
                                          rc_CurData.c_ImportData.c_Core.c_OscTxMessageData,
                                          rc_CurData.c_ImportData.c_Core.c_OscTxSignalData,
                                          rc_CurData.c_ImportData.c_Core.c_EdsOscTxMessageIsSrdo,
                                          rc_CurData.c_ImportData.c_Core.c_WarningMessagesPerTxMessage,
                                          rc_CurData.c_TxMessageOverrideIndices, rc_CurData.u32_OsyNodeIndex, true,
                                          orc_Suffix, oe_ProtocolType, opc_NodeNameReplacement, oq_ReplaceMessageNames,
                                          u32_NewSrdoMessageCount);
      if (ors32_Retval == C_NO_ERR)
      {
         //Rx
         ors32_Retval = mh_GetMessageEntries(oru32_MessageCount, rc_CurData.u32_NewMessageCount, orc_ImportTable,
                                             c_MessagesList,
                                             rc_CurData.c_ImportData.c_Core.c_OscRxMessageData,
                                             rc_CurData.c_ImportData.c_Core.c_OscRxSignalData,
                                             rc_CurData.c_ImportData.c_Core.c_EdsOscRxMessageIsSrdo,
                                             rc_CurData.c_ImportData.c_Core.c_WarningMessagesPerRxMessage,
                                             rc_CurData.c_RxMessageOverrideIndices, rc_CurData.u32_OsyNodeIndex,
                                             false, orc_Suffix, oe_ProtocolType, opc_NodeNameReplacement,
                                             oq_ReplaceMessageNames, u32_NewSrdoMessageCount);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Basic implementation for insert messages action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportReportWidget::m_InsertMessages(void) const
{
   C_CieUtil::h_InsertMessages(this->mc_ImportedAssignedData, this->me_ProtocolType, this->mq_UniqueAddRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Format multiple message entries in import report

   \param[in,out]  oru32_EntryCount             Number of entries in messages table
   \param[out]     oru32_NewEntryCount          New entry count
   \param[in,out]  orc_TableEntries             Current content of messages table
   \param[in,out]  orc_MessageList              All already listed messages
   \param[in]      orc_OscMessageData           Imported core message data for this message type (Tx or Rx)
   \param[in]      orc_OscAllSignalData         All signal data for this message type (Tx or Rx)
   \param[in]      orc_MessageIsSrdo            Flag if message is SRDO
   \param[in]      orc_InfoMessagesPerMessage   Info messages for imported CAN messages for this message type (Tx or Rx)
   \param[in]      orc_MessageOverrideIndices   Indices indicating if message will get replaced or will be added
   \param[in]      ou32_NodeIndex               Node index of this message
   \param[in]      oq_IsTx                      Flag if message is tx
   \param[in]      orc_Suffix                   Suffix of the message file
   \param[in]      oe_ProtocolType              Protocol type
   \param[in]      opc_NodeNameReplacement      Node name replacement
   \param[in]      oq_ReplaceMessageNames       Replace message names
   \param[in,out]  oru32_SrdoIndex              SRDO index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieImportReportWidget::mh_GetMessageEntries(uint32_t & oru32_EntryCount, uint32_t & oru32_NewEntryCount,
                                                      QString & orc_TableEntries,
                                                      std::vector<C_OscCanMessage> & orc_MessageList,
                                                      const std::vector<C_OscCanMessage> & orc_OscMessageData,
                                                      const std::vector<C_OscNodeDataPoolListElement> & orc_OscAllSignalData, const std::vector<uint8_t> & orc_MessageIsSrdo, const
                                                      std::vector<QString> & orc_InfoMessagesPerMessage, const
                                                      std::vector< std::pair<int32_t, int32_t> > & orc_MessageOverrideIndices, const
                                                      uint32_t ou32_NodeIndex, const bool oq_IsTx, const QString & orc_Suffix, const C_OscCanProtocol::E_Type oe_ProtocolType, const stw::scl::C_SclString * const opc_NodeNameReplacement, const bool oq_ReplaceMessageNames,
                                                      uint32_t & oru32_SrdoIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   //Should not happen at this point
   tgl_assert((orc_MessageOverrideIndices.size() == orc_OscMessageData.size()) &&
              (orc_MessageOverrideIndices.size() == orc_InfoMessagesPerMessage.size()));
   if ((orc_MessageOverrideIndices.size() == orc_OscMessageData.size()) &&
       (orc_MessageOverrideIndices.size() == orc_InfoMessagesPerMessage.size()))
   {
      for (uint32_t u32_ItMessage = 0; u32_ItMessage < orc_MessageOverrideIndices.size(); ++u32_ItMessage)
      {
         bool q_AddToTable = true;
         // check if message is already in table
         for (std::vector<C_OscCanMessage>::const_iterator c_MessIt = orc_MessageList.begin();
              c_MessIt != orc_MessageList.end(); ++c_MessIt)
         {
            const C_OscCanMessage & rc_Message = *c_MessIt;
            if (!(rc_Message != orc_OscMessageData[u32_ItMessage]))
            {
               q_AddToTable = false;
               break;
            }
         }

         if (q_AddToTable == true)
         {
            const bool q_IsSrdo = C_CieUtil::h_GetIsSrdoFromMessageIsSrdoVector(orc_MessageIsSrdo, u32_ItMessage);
            orc_TableEntries += "<tr>";
            orc_TableEntries += mh_GetMessageEntry(oru32_EntryCount, orc_OscMessageData[u32_ItMessage],
                                                   orc_OscAllSignalData,
                                                   orc_InfoMessagesPerMessage[u32_ItMessage], ou32_NodeIndex,
                                                   oq_IsTx, q_IsSrdo, u32_ItMessage, orc_Suffix, oe_ProtocolType,
                                                   opc_NodeNameReplacement, oq_ReplaceMessageNames, oru32_SrdoIndex);
            orc_TableEntries += "</tr>";
            ++oru32_EntryCount;
            orc_MessageList.push_back(orc_OscMessageData[u32_ItMessage]);
         }

         // count if message is new for this node (needed later to know if datapool selection is obligatory)
         if (orc_MessageOverrideIndices[u32_ItMessage].second < 0L)
         {
            ++oru32_NewEntryCount;
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Format one message entry in import report

   \param[in]      ou32_Index                Current index (Displayed)
   \param[in]      orc_CurMessage            Current message
   \param[in]      orc_OscAllSignalData      All signal data for this message type (Tx or Rx)
   \param[in]      orc_InfoMessages          All info messages for this message
   \param[in]      ou32_NodeIndex            Node index of this message
   \param[in]      oq_IsTx                   Flag if message is tx
   \param[in]      oq_IsSrdo                 Flag if message is SRDO
   \param[in]      ou32_MessageIndex         Message index of this message
   \param[in]      orc_Suffix                Suffix of the message file
   \param[in]      oe_ProtocolType           Protocol type
   \param[in]      opc_NodeNameReplacement   Node name replacement
   \param[in]      oq_ReplaceMessageNames    Replace message names
   \param[in,out]  oru32_SrdoIndex           SRDO index

   \return
   One message entry in import report
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CieImportReportWidget::mh_GetMessageEntry(const uint32_t ou32_Index, const C_OscCanMessage & orc_CurMessage,
                                                    const std::vector<C_OscNodeDataPoolListElement> & orc_OscAllSignalData, const
                                                    QString & orc_InfoMessages, const uint32_t ou32_NodeIndex, const bool oq_IsTx, const bool oq_IsSrdo, const uint32_t ou32_MessageIndex, const QString & orc_Suffix, const C_OscCanProtocol::E_Type oe_ProtocolType, const stw::scl::C_SclString * const opc_NodeNameReplacement, const bool oq_ReplaceMessageNames,
                                                    uint32_t & oru32_SrdoIndex)
{
   const QString c_TableEntryEnd = "</td>";
   const QString c_LineBreak = "<br/>";
   QString c_Retval;
   QString c_Tmp;

   //Index
   c_Retval += C_CieImportReportWidget::hc_HTML_TABLE_DATA_START + QString::number(ou32_Index + 1U) +
               c_TableEntryEnd;
   //Message name
   if (C_CieImportReportWidget::mh_IsEdsOrDcfImport(orc_Suffix))
   {
      QString c_MessageName = "NodeName";
      if (oq_ReplaceMessageNames)
      {
         const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);
         if (pc_Node != NULL)
         {
            c_MessageName = C_CieUtil::h_GetMessageName(*pc_Node, oq_IsTx, ou32_MessageIndex,
                                                        oe_ProtocolType,
                                                        opc_NodeNameReplacement, oq_IsSrdo, oru32_SrdoIndex);
         }
      }
      else
      {
         c_MessageName = orc_CurMessage.c_Name.c_str();
      }

      c_Retval += C_CieImportReportWidget::hc_HTML_TABLE_DATA_START + c_MessageName + c_TableEntryEnd;
   }
   else
   {
      c_Retval += C_CieImportReportWidget::hc_HTML_TABLE_DATA_START + static_cast<QString>("%1 (0x%2)").arg(
         orc_CurMessage.c_Name.c_str(), QString::number(orc_CurMessage.u32_CanId, 16).toUpper()) + c_TableEntryEnd;
   }

   //Signals
   if (orc_CurMessage.c_Signals.size() == 0UL)
   {
      c_Tmp = C_GtGetText::h_GetText("No Signals");
   }
   else
   {
      c_Tmp = "";
      for (uint32_t u32_ItSignal = 0; u32_ItSignal < orc_CurMessage.c_Signals.size(); ++u32_ItSignal)
      {
         const C_OscCanSignal & rc_CurSignal = orc_CurMessage.c_Signals[u32_ItSignal];
         if (rc_CurSignal.u32_ComDataElementIndex < orc_OscAllSignalData.size())
         {
            const C_OscNodeDataPoolListElement & rc_CurSignalData =
               orc_OscAllSignalData[rc_CurSignal.u32_ComDataElementIndex];
            //Add line break if necessary
            if (u32_ItSignal > 0UL)
            {
               c_Tmp += c_LineBreak;
            }
            c_Tmp += static_cast<QString>(rc_CurSignalData.c_Name.c_str());
         }
      }
   }
   c_Retval += C_CieImportReportWidget::hc_HTML_TABLE_DATA_START + c_Tmp + c_TableEntryEnd;
   //Warnings
   if (orc_InfoMessages.compare("") == 0)
   {
      c_Tmp = "-";
   }
   else
   {
      c_Tmp = orc_InfoMessages;
      c_Tmp = c_Tmp.replace("\n", c_LineBreak);
   }
   c_Retval += C_CieImportReportWidget::hc_HTML_TABLE_DATA_START + c_Tmp + c_TableEntryEnd;
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is eds or dcf import

   \param[in]  orc_Suffix  Suffix

   \return
   Flags

   \retval   True    Is eds or dcf import
   \retval   False   Is not eds or dcf import
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CieImportReportWidget::mh_IsEdsOrDcfImport(const QString & orc_Suffix)
{
   return (orc_Suffix.toUpper() == "EDS") || (orc_Suffix.toUpper() == "DCF");
}
