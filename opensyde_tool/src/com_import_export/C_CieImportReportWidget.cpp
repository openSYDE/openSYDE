//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for system node data pool list import report (implementation)

   Dialog for system node data pool list import report.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QFileInfo>
#include "C_SdUtil.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_OSCLoggingHandler.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"
#include "C_PuiSdUtil.h"
#include "C_OgePopUpDialog.h"
#include "C_CieImportDatapoolSelectWidget.h"
#include "C_CieImportReportWidget.h"
#include "ui_C_CieImportReportWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_CieImportReportWidget::mhc_HTML_TABLE_HEADER_START =
   "<td align=\"left\" valign=\"top\" style=\"padding: 5px 18px 5px 0px;white-space:pre;font-weight:bold;\">";
const QString C_CieImportReportWidget::mhc_HTML_TABLE_DATA_START =
   "<td align=\"left\" valign=\"top\" style=\"padding: 5px 18px 5px 0px;white-space:pre;\">";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent                Reference to parent
   \param[in]      orc_FilePath              Loaded file path
   \param[in]      ou32_BusIndex             Bus index
   \param[in]      oe_ProtocolType           Type of comm protocol
   \param[in]      orc_ImportDataAssigned    Import data assigned
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieImportReportWidget::C_CieImportReportWidget(C_OgePopUpDialog & orc_Parent, const QString & orc_FilePath,
                                                 const uint32 ou32_BusIndex,
                                                 const C_OSCCanProtocol::E_Type oe_ProtocolType,
                                                 const std::vector<C_CieImportDataAssignment> & orc_ImportDataAssigned)
   :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_CieImportReportWidget),
   mrc_ParentDialog(orc_Parent),
   mc_FilePath(orc_FilePath),
   mu32_BusIndex(ou32_BusIndex),
   me_ProtocolType(oe_ProtocolType),
   mc_ImportedAssignedData(orc_ImportDataAssigned)
{
   const QFileInfo c_FileInfo(orc_FilePath);

   mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // set main title
   this->mrc_ParentDialog.SetTitle(QString(C_GtGetText::h_GetText("%1 File Import")).arg(c_FileInfo.completeSuffix().
                                                                                         toUpper()));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Report"));

   //Trigger report
   m_AdaptMessagesToProtocolType();
   m_AdaptMessageNames();
   tgl_assert(m_ShowReport() == C_NO_ERR);

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
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportReportWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return)))
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

   for (uint32 u32_ItNodes = 0; u32_ItNodes < this->mc_ImportedAssignedData.size(); u32_ItNodes++)
   {
      C_CieImportDataAssignment & rc_CurData = this->mc_ImportedAssignedData[u32_ItNodes];
      uint32 u32_NumberCOMMDps =
         C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPools(rc_CurData.u32_OsyNodeIndex, this->me_ProtocolType).size();

      // reset
      rc_CurData.s32_DatapoolIndexForNew = -1;

      // we need a datapool index if there exist new messages
      if (rc_CurData.u32_NewMessageCount > 0)
      {
         // first of all: if no COMM Datapool of this protocol exists add one
         if (u32_NumberCOMMDps == 0)
         {
            // No COM datapool yet. New one must be created if permissible
            if (C_SdUtil::h_CheckDatapoolNumber(rc_CurData.u32_OsyNodeIndex, false, this) == true)
            {
               tgl_assert(C_PuiSdHandler::h_GetInstance()->AddAutoGenCommDataPool(rc_CurData.u32_OsyNodeIndex,
                                                                                  this->me_ProtocolType) == C_NO_ERR);
               u32_NumberCOMMDps++; // we just added one!
            }
            else
            {
               q_Continue = false;
            }
         }

         // if there is exactly one datapool of this protocol type get its index
         if (q_Continue == true)
         {
            if (u32_NumberCOMMDps == 1)
            {
               const C_OSCNode * const pc_Node =
                  C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(rc_CurData.u32_OsyNodeIndex);

               // get datapool index of the corresponding datapool
               if (pc_Node != NULL)
               {
                  for (uint32 u32_ItDatapool = 0; u32_ItDatapool < pc_Node->c_DataPools.size(); ++u32_ItDatapool)
                  {
                     if (pc_Node->c_DataPools[u32_ItDatapool].e_Type == C_OSCNodeDataPool::eCOM)
                     {
                        if (C_PuiSdUtil::h_GetRelatedCANProtocolType(rc_CurData.u32_OsyNodeIndex,
                                                                     u32_ItDatapool) == this->me_ProtocolType)
                        {
                           rc_CurData.s32_DatapoolIndexForNew = static_cast<sint32>(u32_ItDatapool);
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

               QPointer<C_OgePopUpDialog> const c_Popup = new C_OgePopUpDialog(this, this);
               C_CieImportDatapoolSelectWidget * const pc_DialogImportReport =
                  new C_CieImportDatapoolSelectWidget(*c_Popup, rc_CurData.u32_OsyNodeIndex, this->me_ProtocolType);

               // resize
               c_Popup->SetSize(QSize(700, 400));

               // execute datapool popup
               if (c_Popup->exec() == static_cast<sintn>(QDialog::Accepted))
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
               }
               //lint -e{429}  no memory leak because of the parent of pc_DialogImportReport and the Qt memory
               // management
            }
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
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportReportWidget::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt all stored message names as specified

   * Eliminate spaces
   * Cut after 31 characters
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportReportWidget::m_AdaptMessageNames(void)
{
   //Each imported node
   for (uint32 u32_ItNodes = 0; u32_ItNodes < this->mc_ImportedAssignedData.size(); u32_ItNodes++)
   {
      uint32 u32_Count;
      C_CieImportDataAssignment & rc_CurData = this->mc_ImportedAssignedData[u32_ItNodes];

      //Each message
      for (u32_Count = 0; u32_Count < rc_CurData.c_ImportData.c_Core.c_OSCRxMessageData.size(); ++u32_Count)
      {
         C_OSCCanMessage & rc_CurMessage = rc_CurData.c_ImportData.c_Core.c_OSCRxMessageData[u32_Count];
         mh_AdaptName(rc_CurMessage.c_Name, rc_CurMessage.c_Comment);
      }
      for (u32_Count = 0; u32_Count < rc_CurData.c_ImportData.c_Core.c_OSCTxMessageData.size(); ++u32_Count)
      {
         C_OSCCanMessage & rc_CurMessage = rc_CurData.c_ImportData.c_Core.c_OSCTxMessageData[u32_Count];
         mh_AdaptName(rc_CurMessage.c_Name, rc_CurMessage.c_Comment);
      }
      //Each signal
      for (u32_Count = 0; u32_Count < rc_CurData.c_ImportData.c_Core.c_OSCRxSignalData.size(); ++u32_Count)
      {
         C_OSCNodeDataPoolListElement & rc_CurSignal = rc_CurData.c_ImportData.c_Core.c_OSCRxSignalData[u32_Count];
         mh_AdaptName(rc_CurSignal.c_Name, rc_CurSignal.c_Comment);
      }
      for (u32_Count = 0; u32_Count < rc_CurData.c_ImportData.c_Core.c_OSCTxSignalData.size(); ++u32_Count)
      {
         C_OSCNodeDataPoolListElement & rc_CurSignal = rc_CurData.c_ImportData.c_Core.c_OSCTxSignalData[u32_Count];
         mh_AdaptName(rc_CurSignal.c_Name, rc_CurSignal.c_Comment);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt all stored messages to protocol restrictions if necessary
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportReportWidget::m_AdaptMessagesToProtocolType(void)
{
   //Each imported node
   for (uint32 u32_ItNodes = 0; u32_ItNodes < this->mc_ImportedAssignedData.size(); u32_ItNodes++)
   {
      C_CieImportDataAssignment & rc_CurData = this->mc_ImportedAssignedData[u32_ItNodes];
      //Each Rx message
      for (uint32 u32_ItMessage = 0;
           u32_ItMessage < rc_CurData.c_ImportData.c_Core.c_OSCRxMessageData.size();
           ++u32_ItMessage)
      {
         QStringList c_Infos;
         QString & rc_CurInfoMessage = rc_CurData.c_ImportData.c_Core.c_WarningMessagesPerRxMessage[u32_ItMessage];
         C_OSCCanMessage & rc_CurMessage = rc_CurData.c_ImportData.c_Core.c_OSCRxMessageData[u32_ItMessage];
         C_SdUtil::h_AdaptMessageToProtocolType(rc_CurMessage, this->me_ProtocolType, &c_Infos);
         //Add info
         if (rc_CurInfoMessage.isEmpty() == false)
         {
            rc_CurInfoMessage += "\n";
         }
         rc_CurInfoMessage += c_Infos.join("\n");
      }
      //Each Tx message
      for (uint32 u32_ItMessage = 0; u32_ItMessage < rc_CurData.c_ImportData.c_Core.c_OSCTxMessageData.size();
           ++u32_ItMessage)
      {
         QStringList c_Infos;
         QString & rc_CurInfoMessage = rc_CurData.c_ImportData.c_Core.c_WarningMessagesPerTxMessage[u32_ItMessage];
         C_OSCCanMessage & rc_CurMessage = rc_CurData.c_ImportData.c_Core.c_OSCTxMessageData[u32_ItMessage];
         C_SdUtil::h_AdaptMessageToProtocolType(rc_CurMessage, this->me_ProtocolType, &c_Infos);
         //Add info
         if (rc_CurInfoMessage.isEmpty() == false)
         {
            rc_CurInfoMessage += "\n";
         }
         rc_CurInfoMessage += c_Infos.join("\n");
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt name as specified

   * Eliminate spaces
   * Cut after 31 characters

   \param[in,out]  orc_Name      Old/New name
   \param[in,out]  orc_Comment   Comment
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportReportWidget::mh_AdaptName(stw_scl::C_SCLString & orc_Name, stw_scl::C_SCLString & orc_Comment)
{
   stw_scl::C_SCLString c_NewName;
   //Remove spaces
   for (uint32 u32_ItOldName = 0; u32_ItOldName < orc_Name.Length(); ++u32_ItOldName)
   {
      if (orc_Name[static_cast<sintn>(u32_ItOldName + 1UL)] == ' ')
      {
         //Skip
      }
      else
      {
         c_NewName += orc_Name[static_cast<sintn>(u32_ItOldName + 1UL)];
         //Cut string
         if (c_NewName.Length() >= msn_C_ITEM_MAX_CHAR_COUNT)
         {
            break;
         }
      }
   }
   if (orc_Name == c_NewName)
   {
      //No change
   }
   else
   {
      stw_scl::C_SCLString c_Addition;
      if (orc_Comment != "")
      {
         c_Addition = "\n";
      }
      //Set comment before overwriting the original name
      //Translation: 1=Original name from file,2=Optional line break if there was some more content
      orc_Comment +=
         QString(C_GtGetText::h_GetText("%2Original name (from import source): %1")).arg(orc_Name.c_str()).arg(
            c_Addition.c_str()).toStdString().c_str();
      orc_Name = c_NewName;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show/update report

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieImportReportWidget::m_ShowReport(void)
{
   sint32 s32_Retval;
   QString c_Table = "";
   uint32 u32_MessageCount = 0U;

   //Check
   s32_Retval = m_CheckMessages();
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = m_CheckMessageMatches();
   }

   // Get table with message entries
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_OSCCanMessage> c_MessagesList; // for listing every message only once

      //Each imported node
      for (uint32 u32_ItNodes = 0; (u32_ItNodes < this->mc_ImportedAssignedData.size()) && (s32_Retval == C_NO_ERR);
           u32_ItNodes++)
      {
         C_CieImportDataAssignment & rc_CurData = this->mc_ImportedAssignedData[u32_ItNodes];

         rc_CurData.u32_NewMessageCount = 0UL;

         //Tx
         s32_Retval = mh_GetMessageEntries(u32_MessageCount, rc_CurData.u32_NewMessageCount, c_Table, c_MessagesList,
                                           rc_CurData.c_ImportData.c_Core.c_OSCTxMessageData,
                                           rc_CurData.c_ImportData.c_Core.c_OSCTxSignalData,
                                           rc_CurData.c_ImportData.c_Core.c_WarningMessagesPerTxMessage,
                                           rc_CurData.c_TxMessageOverrideIndices);
         if (s32_Retval == C_NO_ERR)
         {
            //Rx
            s32_Retval = mh_GetMessageEntries(u32_MessageCount, rc_CurData.u32_NewMessageCount, c_Table, c_MessagesList,
                                              rc_CurData.c_ImportData.c_Core.c_OSCRxMessageData,
                                              rc_CurData.c_ImportData.c_Core.c_OSCRxSignalData,
                                              rc_CurData.c_ImportData.c_Core.c_WarningMessagesPerRxMessage,
                                              rc_CurData.c_RxMessageOverrideIndices);
         }
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      QString c_CompleteLog;

      //Start
      c_CompleteLog += "<html>";
      c_CompleteLog += "<body>";
      //File
      c_CompleteLog += "<h3>";
      c_CompleteLog += C_GtGetText::h_GetText("Source File Information");
      c_CompleteLog += "</h3>";
      c_CompleteLog += "<table><tr>";
      c_CompleteLog += C_CieImportReportWidget::mhc_HTML_TABLE_DATA_START;
      c_CompleteLog += C_GtGetText::h_GetText("Path:");
      c_CompleteLog += "</td>";
      c_CompleteLog += C_CieImportReportWidget::mhc_HTML_TABLE_DATA_START;
      c_CompleteLog += QString("<a href=\"file:%1\"><span style=\"color: %2;\">%3</span></a>").
                       arg(this->mc_FilePath).
                       arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                       arg(this->mc_FilePath);
      c_CompleteLog += "</td></tr><tr>";

      //Content
      c_CompleteLog += C_CieImportReportWidget::mhc_HTML_TABLE_DATA_START;
      c_CompleteLog += C_GtGetText::h_GetText("Read Content:");
      c_CompleteLog += "</td>";
      c_CompleteLog += C_CieImportReportWidget::mhc_HTML_TABLE_DATA_START;
      //Add node names
      c_CompleteLog += C_GtGetText::h_GetText("Nodes: ");
      for (uint32 u32_ItNodes = 0; u32_ItNodes < this->mc_ImportedAssignedData.size(); u32_ItNodes++)
      {
         C_CieImportDataAssignment & rc_CurData = this->mc_ImportedAssignedData[u32_ItNodes];
         c_CompleteLog += rc_CurData.c_ImportData.c_NodeName;
         c_CompleteLog += QString(C_GtGetText::h_GetText(" (%1 Tx / %2 Rx); ")).
                          arg(rc_CurData.c_ImportData.c_Core.c_OSCTxMessageData.size()).
                          arg(rc_CurData.c_ImportData.c_Core.c_OSCRxMessageData.size());
      }
      c_CompleteLog += "<br/>";
      c_CompleteLog += C_GtGetText::h_GetText("For parsing errors, warnings and detailed information see ");
      c_CompleteLog += QString("<a href=\"file:%1\"><span style=\"color: %2;\">%3</span></a>.").
                       arg(C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str()).
                       arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                       arg(C_GtGetText::h_GetText("log file"));
      c_CompleteLog += "</td></tr></table>";

      //Update log file
      C_OSCLoggingHandler::h_Flush();

      //Info
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
      c_CompleteLog += C_GtGetText::h_GetText("Cut to 31 characters");
      c_CompleteLog += "</p>";

      //Table

      if (c_Table.isEmpty() == false)
      {
         //New table
         c_CompleteLog += "<p><b>";
         //Translation: 1=Number of messages
         c_CompleteLog += QString(C_GtGetText::h_GetText("Import Messages (%1)")).arg(u32_MessageCount);
         c_CompleteLog += "</b></p>";
         c_CompleteLog += "<table>";
         //Header
         c_CompleteLog += "<tr>";
         c_CompleteLog += C_CieImportReportWidget::mhc_HTML_TABLE_HEADER_START;
         c_CompleteLog += "#</td>";
         c_CompleteLog += C_CieImportReportWidget::mhc_HTML_TABLE_HEADER_START;
         c_CompleteLog += C_GtGetText::h_GetText("Name");
         c_CompleteLog += "</td>";
         c_CompleteLog += C_CieImportReportWidget::mhc_HTML_TABLE_HEADER_START;
         c_CompleteLog += C_GtGetText::h_GetText("Signals");
         c_CompleteLog += "</td>";
         c_CompleteLog += C_CieImportReportWidget::mhc_HTML_TABLE_HEADER_START;
         c_CompleteLog += C_GtGetText::h_GetText("Warnings");
         c_CompleteLog += "</td>";
         c_CompleteLog += "</tr>";
         //Content
         c_CompleteLog += c_Table;
         c_CompleteLog += "</table>";
      }

      //End
      c_CompleteLog += "</body>";
      c_CompleteLog += "</html>";
      this->mpc_Ui->pc_TextBrowserReport->setHtml(c_CompleteLog);
   }

   if (s32_Retval != C_NO_ERR)
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check messages

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieImportReportWidget::m_CheckMessages(void) const
{
   sint32 s32_Retval = C_NO_ERR;

   //Each imported node
   for (uint32 u32_ItNodes = 0; (u32_ItNodes < this->mc_ImportedAssignedData.size()) && (s32_Retval == C_NO_ERR);
        u32_ItNodes++)
   {
      const C_CieImportDataAssignment & rc_CurData = this->mc_ImportedAssignedData[u32_ItNodes];

      if (rc_CurData.c_ImportData.c_Core.c_OSCRxMessageData.size() !=
          rc_CurData.c_ImportData.c_Ui.c_UiRxMessageData.size())
      {
         s32_Retval = C_CONFIG;
      }
      else
      {
         uint32 u32_SignalSum = 0;
         for (uint32 u32_ItMessage = 0;
              (u32_ItMessage < rc_CurData.c_ImportData.c_Core.c_OSCRxMessageData.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItMessage)
         {
            const C_OSCCanMessage & rc_OSCCurMessage = rc_CurData.c_ImportData.c_Core.c_OSCRxMessageData[u32_ItMessage];
            const C_PuiSdNodeCanMessage & rc_UiCurMessage =
               rc_CurData.c_ImportData.c_Ui.c_UiRxMessageData[u32_ItMessage];
            if (rc_OSCCurMessage.c_Signals.size() != rc_UiCurMessage.c_Signals.size())
            {
               s32_Retval = C_CONFIG;
            }
            for (uint32 u32_ItSignal = 0;
                 (u32_ItSignal < rc_OSCCurMessage.c_Signals.size()) && (s32_Retval == C_NO_ERR); ++u32_ItSignal)
            {
               const C_OSCCanSignal & rc_CurSignal = rc_OSCCurMessage.c_Signals[u32_ItSignal];
               if (rc_CurSignal.u32_ComDataElementIndex >= rc_CurData.c_ImportData.c_Core.c_OSCRxSignalData.size())
               {
                  s32_Retval = C_CONFIG;
               }
            }
            u32_SignalSum += rc_OSCCurMessage.c_Signals.size();
         }
         if (rc_CurData.c_ImportData.c_Core.c_OSCRxSignalData.size() != u32_SignalSum)
         {
            s32_Retval = C_CONFIG;
         }
      }
      if (rc_CurData.c_ImportData.c_Core.c_OSCTxMessageData.size() !=
          rc_CurData.c_ImportData.c_Ui.c_UiTxMessageData.size())
      {
         s32_Retval = C_CONFIG;
      }
      else
      {
         uint32 u32_SignalSum = 0;
         for (uint32 u32_ItMessage = 0;
              (u32_ItMessage < rc_CurData.c_ImportData.c_Core.c_OSCTxMessageData.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItMessage)
         {
            const C_OSCCanMessage & rc_OSCCurMessage = rc_CurData.c_ImportData.c_Core.c_OSCTxMessageData[u32_ItMessage];
            const C_PuiSdNodeCanMessage & rc_UiCurMessage =
               rc_CurData.c_ImportData.c_Ui.c_UiTxMessageData[u32_ItMessage];
            if (rc_OSCCurMessage.c_Signals.size() != rc_UiCurMessage.c_Signals.size())
            {
               s32_Retval = C_CONFIG;
            }
            for (uint32 u32_ItSignal = 0;
                 (u32_ItSignal < rc_OSCCurMessage.c_Signals.size()) && (s32_Retval == C_NO_ERR); ++u32_ItSignal)
            {
               const C_OSCCanSignal & rc_CurSignal = rc_OSCCurMessage.c_Signals[u32_ItSignal];
               if (rc_CurSignal.u32_ComDataElementIndex >= rc_CurData.c_ImportData.c_Core.c_OSCTxSignalData.size())
               {
                  s32_Retval = C_CONFIG;
               }
            }
            u32_SignalSum += rc_OSCCurMessage.c_Signals.size();
         }
         if (rc_CurData.c_ImportData.c_Core.c_OSCTxSignalData.size() != u32_SignalSum)
         {
            s32_Retval = C_CONFIG;
         }
      }
      if (rc_CurData.c_ImportData.c_Core.c_OSCRxSignalData.size() !=
          rc_CurData.c_ImportData.c_Ui.c_UiRxSignalData.size())
      {
         s32_Retval = C_CONFIG;
      }
      if (rc_CurData.c_ImportData.c_Core.c_OSCTxSignalData.size() !=
          rc_CurData.c_ImportData.c_Ui.c_UiTxSignalData.size())
      {
         s32_Retval = C_CONFIG;
      }
      if (rc_CurData.c_ImportData.c_Core.c_OSCRxMessageData.size() !=
          rc_CurData.c_ImportData.c_Core.c_WarningMessagesPerRxMessage.size())
      {
         s32_Retval = C_CONFIG;
      }
      if (rc_CurData.c_ImportData.c_Core.c_OSCTxMessageData.size() !=
          rc_CurData.c_ImportData.c_Core.c_WarningMessagesPerTxMessage.size())
      {
         s32_Retval = C_CONFIG;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check for matching messages

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieImportReportWidget::m_CheckMessageMatches(void)
{
   sint32 s32_Retval = C_NO_ERR;

   //Each imported node
   for (uint32 u32_ItNodes = 0; (u32_ItNodes < this->mc_ImportedAssignedData.size()) && (s32_Retval == C_NO_ERR);
        u32_ItNodes++)
   {
      C_CieImportDataAssignment & rc_CurData = this->mc_ImportedAssignedData[u32_ItNodes];

      // Make sure these are empty
      rc_CurData.c_RxMessageOverrideIndices.clear();
      rc_CurData.c_TxMessageOverrideIndices.clear();

      std::vector<stw_opensyde_core::C_OSCCanMessage>::const_iterator c_DBCMessageIt;
      std::pair<sint32, sint32> c_OverrideInfo;

      // Check every Tx message of DBC
      for (c_DBCMessageIt = rc_CurData.c_ImportData.c_Core.c_OSCTxMessageData.begin();
           (c_DBCMessageIt != rc_CurData.c_ImportData.c_Core.c_OSCTxMessageData.end()) && (s32_Retval == C_NO_ERR);
           ++c_DBCMessageIt)
      {
         s32_Retval = m_GetMessageOverrideInfo(*c_DBCMessageIt, true, rc_CurData.u32_OsyNodeIndex,
                                               rc_CurData.u32_OsyInterfaceIndex, c_OverrideInfo);
         rc_CurData.c_TxMessageOverrideIndices.push_back(c_OverrideInfo);
      }

      // Check every Rx message of DBC
      for (c_DBCMessageIt = rc_CurData.c_ImportData.c_Core.c_OSCRxMessageData.begin();
           (c_DBCMessageIt != rc_CurData.c_ImportData.c_Core.c_OSCRxMessageData.end()) && (s32_Retval == C_NO_ERR);
           ++c_DBCMessageIt)
      {
         s32_Retval = m_GetMessageOverrideInfo(*c_DBCMessageIt, false, rc_CurData.u32_OsyNodeIndex,
                                               rc_CurData.u32_OsyInterfaceIndex, c_OverrideInfo);
         rc_CurData.c_RxMessageOverrideIndices.push_back(c_OverrideInfo);
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Compare a DBC message with existing messages to find out if the message is new or just an update.

   \param[in]   orc_DBCMessageToFind      message from DBC file that should be checked for existing message
   \param[in]   oq_Tx                     flag if message is Tx
   \param[in]   ou32_OsyNodeIndex         Osy node index
   \param[in]   ou32_OsyInterfaceIndex    Osy interface index
   \param[out]  orc_MessageOverrideInfo   pair of datapool index and index of message in corresponding protocol

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieImportReportWidget::m_GetMessageOverrideInfo(const C_OSCCanMessage & orc_DBCMessageToFind, const bool oq_Tx,
                                                         const uint32 ou32_OsyNodeIndex,
                                                         const uint32 ou32_OsyInterfaceIndex,
                                                         std::pair<stw_types::sint32,
                                                                   stw_types::sint32> & orc_MessageOverrideInfo)
const
{
   sint32 s32_Retval = C_NO_ERR;

   // reset
   orc_MessageOverrideInfo = std::make_pair(-1, -1);

   const C_OSCNode * pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_OsyNodeIndex);

   if (pc_Node != NULL)
   {
      std::vector<const C_OSCCanProtocol *> c_Protocols = pc_Node->GetCANProtocolsConst(this->me_ProtocolType);
      std::vector<const C_OSCCanProtocol *>::const_iterator c_ProtocolIt;

      // Iterate Protocols (resp. Datapools)
      for (c_ProtocolIt = c_Protocols.begin(); c_ProtocolIt != c_Protocols.end(); ++c_ProtocolIt)
      {
         const C_OSCCanProtocol * const pc_Protocol = *c_ProtocolIt;
         if (pc_Protocol != NULL)
         {
            if (ou32_OsyInterfaceIndex < pc_Protocol->c_ComMessages.size())
            {
               const C_OSCCanMessageContainer & rc_MessageContainer =
                  pc_Protocol->c_ComMessages[ou32_OsyInterfaceIndex];
               const std::vector<C_OSCCanMessage> & rc_Messages = rc_MessageContainer.GetMessagesConst(oq_Tx);

               // Check all existing messages for a match (CAN Id and Name)
               for (uint32 u32_MessageIt = 0; u32_MessageIt < rc_Messages.size(); u32_MessageIt++)
               {
                  const C_OSCCanMessage & rc_ExistingMessage = rc_Messages[u32_MessageIt];
                  if ((rc_ExistingMessage.u32_CanId == orc_DBCMessageToFind.u32_CanId) &&
                      (rc_ExistingMessage.c_Name == orc_DBCMessageToFind.c_Name))
                  {
                     orc_MessageOverrideInfo =
                        std::make_pair(static_cast<sint32>(pc_Protocol->u32_DataPoolIndex),
                                       static_cast<sint32>(u32_MessageIt));
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

   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Basic implementation for insert messages action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportReportWidget::m_InsertMessages(void) const
{
   //Each imported node
   for (uint32 u32_ItNodes = 0; u32_ItNodes < this->mc_ImportedAssignedData.size(); u32_ItNodes++)
   {
      const C_CieImportDataAssignment & rc_CurData = this->mc_ImportedAssignedData[u32_ItNodes];

      tgl_assert(rc_CurData.s32_DatapoolIndexForNew >= 0);
      if (rc_CurData.s32_DatapoolIndexForNew >= 0)
      {
         // Insert Tx messages
         sint32 s32_Retval = mh_InsertMessages(rc_CurData.u32_OsyNodeIndex, this->me_ProtocolType,
                                               rc_CurData.u32_OsyInterfaceIndex,
                                               rc_CurData.s32_DatapoolIndexForNew, true,
                                               rc_CurData.c_ImportData.c_Core.c_OSCTxMessageData,
                                               rc_CurData.c_ImportData.c_Ui.c_UiTxMessageData,
                                               rc_CurData.c_ImportData.c_Core.c_OSCTxSignalData,
                                               rc_CurData.c_ImportData.c_Ui.c_UiTxSignalData,
                                               rc_CurData.c_TxMessageOverrideIndices);

         tgl_assert(s32_Retval == C_NO_ERR);
         if (s32_Retval == C_NO_ERR)
         {
            // Insert Rx messages
            s32_Retval = mh_InsertMessages(rc_CurData.u32_OsyNodeIndex, this->me_ProtocolType,
                                           rc_CurData.u32_OsyInterfaceIndex,
                                           rc_CurData.s32_DatapoolIndexForNew, false,
                                           rc_CurData.c_ImportData.c_Core.c_OSCRxMessageData,
                                           rc_CurData.c_ImportData.c_Ui.c_UiRxMessageData,
                                           rc_CurData.c_ImportData.c_Core.c_OSCRxSignalData,
                                           rc_CurData.c_ImportData.c_Ui.c_UiRxSignalData,
                                           rc_CurData.c_RxMessageOverrideIndices);
            tgl_assert(s32_Retval == C_NO_ERR);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Format multiple message entries in import report

   \param[in,out]  oru32_EntryCount             Number of entries in messages table
   \param[out]     oru32_NewEntryCount          New entry count
   \param[in,out]  orc_TableEntries             Current content of messages table
   \param[in,out]  orc_MessageList              All already listed messages
   \param[in]      orc_OSCMessageData           Imported core message data for this message type (Tx or Rx)
   \param[in]      orc_OSCAllSignalData         All signal data for this message type (Tx or Rx)
   \param[in]      orc_InfoMessagesPerMessage   Info messages for imported CAN messages for this message type (Tx or Rx)
   \param[in]      orc_MessageOverrideIndices   Indices indicating if message will get replaced or will be added

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieImportReportWidget::mh_GetMessageEntries(uint32 & oru32_EntryCount, uint32 & oru32_NewEntryCount,
                                                     QString & orc_TableEntries,
                                                     std::vector<C_OSCCanMessage> & orc_MessageList,
                                                     const std::vector<C_OSCCanMessage> & orc_OSCMessageData,
                                                     const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCAllSignalData, const std::vector<QString> & orc_InfoMessagesPerMessage, const std::vector< std::pair<stw_types::sint32,
                                                                                                                                                                                                                   stw_types::sint32> > & orc_MessageOverrideIndices)
{
   sint32 s32_Retval = C_NO_ERR;

   //Should not happen at this point
   tgl_assert((orc_MessageOverrideIndices.size() == orc_OSCMessageData.size()) &&
              (orc_MessageOverrideIndices.size() == orc_InfoMessagesPerMessage.size()));
   if ((orc_MessageOverrideIndices.size() == orc_OSCMessageData.size()) &&
       (orc_MessageOverrideIndices.size() == orc_InfoMessagesPerMessage.size()))
   {
      for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_MessageOverrideIndices.size(); ++u32_ItMessage)
      {
         bool q_AddToTable = true;
         // check if message is already in table
         for (std::vector<C_OSCCanMessage>::const_iterator c_MessIt = orc_MessageList.begin();
              c_MessIt != orc_MessageList.end(); ++c_MessIt)
         {
            const C_OSCCanMessage & rc_Message = *c_MessIt;
            if (!(rc_Message != orc_OSCMessageData[u32_ItMessage]))
            {
               q_AddToTable = false;
               break;
            }
         }

         if (q_AddToTable == true)
         {
            orc_TableEntries += "<tr>";
            orc_TableEntries += mh_GetMessageEntry(oru32_EntryCount, orc_OSCMessageData[u32_ItMessage],
                                                   orc_OSCAllSignalData,
                                                   orc_InfoMessagesPerMessage[u32_ItMessage]);
            orc_TableEntries += "</tr>";
            ++oru32_EntryCount;
            orc_MessageList.push_back(orc_OSCMessageData[u32_ItMessage]);
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

   \param[in]  ou32_Index              Current index (Displayed)
   \param[in]  orc_CurMessage          Current message
   \param[in]  orc_OSCAllSignalData    All signal data for this message type (Tx or Rx)
   \param[in]  orc_InfoMessages        All info messages for this message

   \return
   One message entry in import report
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CieImportReportWidget::mh_GetMessageEntry(const uint32 ou32_Index, const C_OSCCanMessage & orc_CurMessage,
                                                    const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCAllSignalData,
                                                    const QString & orc_InfoMessages)
{
   const QString c_TableEntryEnd = "</td>";
   const QString c_LineBreak = "<br/>";
   QString c_Retval;
   QString c_Tmp;

   //Index
   c_Retval += C_CieImportReportWidget::mhc_HTML_TABLE_DATA_START + QString::number(ou32_Index + 1UL) +
               c_TableEntryEnd;
   //Message name
   //Translation: 1=CAN Message name, 2=CAN Message ID
   c_Retval += C_CieImportReportWidget::mhc_HTML_TABLE_DATA_START + QString("%1 (0x%2)").arg(
      orc_CurMessage.c_Name.c_str()).arg(orc_CurMessage.u32_CanId, 0, 16) + c_TableEntryEnd;
   //Signals
   if (orc_CurMessage.c_Signals.size() == 0UL)
   {
      c_Tmp = C_GtGetText::h_GetText("No Signals");
   }
   else
   {
      c_Tmp = "";
      for (uint32 u32_ItSignal = 0; u32_ItSignal < orc_CurMessage.c_Signals.size(); ++u32_ItSignal)
      {
         const C_OSCCanSignal & rc_CurSignal = orc_CurMessage.c_Signals[u32_ItSignal];
         if (rc_CurSignal.u32_ComDataElementIndex < orc_OSCAllSignalData.size())
         {
            const C_OSCNodeDataPoolListElement & rc_CurSignalData =
               orc_OSCAllSignalData[rc_CurSignal.u32_ComDataElementIndex];
            //Add line break if necessary
            if (u32_ItSignal > 0UL)
            {
               c_Tmp += c_LineBreak;
            }
            c_Tmp += QString(rc_CurSignalData.c_Name.c_str());
         }
      }
   }
   c_Retval += C_CieImportReportWidget::mhc_HTML_TABLE_DATA_START + c_Tmp + c_TableEntryEnd;
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
   c_Retval += C_CieImportReportWidget::mhc_HTML_TABLE_DATA_START + c_Tmp + c_TableEntryEnd;
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert all specified messages into the specified message container

   \param[in]  ou32_NodeIndex                   Node index (ID)
   \param[in]  oe_Type                          COMM protocol type (ID)
   \param[in]  ou32_InterfaceIndex              Interface index (ID)
   \param[in]  ou32_DatapoolIndexForNewContent  Datapool index for new content
   \param[in]  oq_MessagesAreTx                 Flag if messages are Tx (ID)
   \param[in]  orc_OSCMessageData               Imported core message data
   \param[in]  orc_UiMessageData                Imported UI message data
   \param[in]  orc_OSCSignalData                Imported core signal data
   \param[in]  orc_UiSignalData                 Imported UI signal data
   \param[in]  orc_MessageOverrideIndices       Indices indicating if message will get replaced or will be added

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieImportReportWidget::mh_InsertMessages(const uint32 ou32_NodeIndex, const C_OSCCanProtocol::E_Type oe_Type,
                                                  const uint32 ou32_InterfaceIndex,
                                                  const uint32 ou32_DatapoolIndexForNewContent,
                                                  const bool oq_MessagesAreTx,
                                                  const std::vector<C_OSCCanMessage> & orc_OSCMessageData,
                                                  const std::vector<C_PuiSdNodeCanMessage> & orc_UiMessageData,
                                                  const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalData,
                                                  const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalData,
                                                  const std::vector< std::pair<stw_types::sint32,
                                                                               stw_types::sint32> > & orc_MessageOverrideIndices)
{
   sint32 s32_Retval = C_NO_ERR;

   if (((orc_OSCMessageData.size() == orc_MessageOverrideIndices.size()) &&
        (orc_OSCSignalData.size() == orc_UiSignalData.size())) &&
       (orc_OSCMessageData.size() == orc_UiMessageData.size()))
   {
      //Get relevant lists
      const C_OSCNodeDataPoolList * pc_CurList = NULL;
      const C_OSCNode * pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);
      if (pc_Node != NULL)
      {
         for (uint32 u32_ItProtocol = 0; u32_ItProtocol < pc_Node->c_ComProtocols.size(); ++u32_ItProtocol)
         {
            const C_OSCCanProtocol & rc_ComProtocol = pc_Node->c_ComProtocols[u32_ItProtocol];
            if (rc_ComProtocol.u32_DataPoolIndex < pc_Node->c_DataPools.size())
            {
               const C_OSCNodeDataPool & rc_CurDataPool = pc_Node->c_DataPools[rc_ComProtocol.u32_DataPoolIndex];
               pc_CurList = C_OSCCanProtocol::h_GetComListConst(rc_CurDataPool, ou32_InterfaceIndex, oq_MessagesAreTx);
            }
         }
      }
      for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_OSCMessageData.size(); ++u32_ItMessage)
      {
         const C_OSCCanMessage & rc_CurMessage = orc_OSCMessageData[u32_ItMessage];
         const C_PuiSdNodeCanMessage & rc_UiMessage = orc_UiMessageData[u32_ItMessage];
         std::vector<C_OSCNodeDataPoolListElement> c_CurOSCSignalCommons;
         std::vector<C_PuiSdNodeDataPoolListElement> c_CurUISignalCommons;

         c_CurOSCSignalCommons.reserve(rc_CurMessage.c_Signals.size());
         c_CurUISignalCommons.reserve(rc_CurMessage.c_Signals.size());
         //Copy all relevant signals
         for (uint32 u32_ItSignal = 0; u32_ItSignal < rc_CurMessage.c_Signals.size(); ++u32_ItSignal)
         {
            const C_OSCCanSignal & rc_CurSignal = rc_CurMessage.c_Signals[u32_ItSignal];
            if (rc_CurSignal.u32_ComDataElementIndex < orc_OSCSignalData.size())
            {
               C_OSCNodeDataPoolListElement c_Copy = orc_OSCSignalData[rc_CurSignal.u32_ComDataElementIndex];
               if (pc_CurList != NULL)
               {
                  C_OSCNodeDataPoolContent c_DataSet;
                  //Handle data sets
                  c_DataSet.SetArray(c_Copy.GetArray());
                  c_DataSet.SetType(c_Copy.GetType());
                  tgl_assert(pc_CurList->c_DataSets.size() == 1UL);
                  c_Copy.c_DataSetValues.resize(pc_CurList->c_DataSets.size(), c_DataSet);
               }
               c_CurOSCSignalCommons.push_back(c_Copy);
               c_CurUISignalCommons.push_back(orc_UiSignalData[rc_CurSignal.u32_ComDataElementIndex]);
            }
         }
         if (orc_MessageOverrideIndices[u32_ItMessage].second < 0L)
         {
            //Insert new messages
            if (C_PuiSdHandler::h_GetInstance()->AddCanMessage(ou32_NodeIndex, oe_Type, ou32_InterfaceIndex,
                                                               ou32_DatapoolIndexForNewContent, oq_MessagesAreTx,
                                                               rc_CurMessage,
                                                               c_CurOSCSignalCommons, c_CurUISignalCommons,
                                                               rc_UiMessage, false) != C_NO_ERR)
            {
               s32_Retval = C_RANGE;
            }
         }
         else
         {
            //Get updated node
            pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);
            if (pc_Node != NULL)
            {
               // Message already exists, adapt message in its datapool
               uint32 u32_PresentDatapoolIndex = orc_MessageOverrideIndices[u32_ItMessage].first;

               const stw_opensyde_core::C_OSCCanMessageContainer * pc_Container =
                  C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(ou32_NodeIndex, oe_Type,
                                                                                  ou32_InterfaceIndex,
                                                                                  u32_PresentDatapoolIndex);
               if ((pc_Container != NULL) && (ou32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size()))
               {
                  const C_OSCCanMessageIdentificationIndices c_Id(
                     ou32_NodeIndex, oe_Type, ou32_InterfaceIndex, u32_PresentDatapoolIndex, oq_MessagesAreTx,
                     static_cast<uint32>(orc_MessageOverrideIndices[u32_ItMessage].second));
                  const C_OSCNodeComInterfaceSettings & rc_Interface =
                     pc_Node->c_Properties.c_ComInterfaces[ou32_InterfaceIndex];
                  if ((rc_Interface.q_IsBusConnected == true) && (pc_Container->q_IsComProtocolUsedByInterface == true))
                  {
                     const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_Id);
                     if (pc_Message != NULL)
                     {
                        C_PuiSdNodeCanMessageSyncManager c_SyncManager;
                        c_SyncManager.Init(rc_Interface.u32_BusIndex, oe_Type);
                        //Delete previous signals
                        for (uint32 u32_ItSignal = pc_Message->c_Signals.size(); u32_ItSignal > 0UL; --u32_ItSignal)
                        {
                           if (c_SyncManager.DeleteCanSignal(c_Id, u32_ItSignal - 1UL) != C_NO_ERR)
                           {
                              s32_Retval = C_RANGE;
                           }
                        }
                        //Add signals (BEFORE replacing the message)
                        for (uint32 u32_ItSignal = 0; u32_ItSignal < rc_CurMessage.c_Signals.size(); ++u32_ItSignal)
                        {
                           if (c_SyncManager.AddCanSignal(c_Id, rc_CurMessage.c_Signals[u32_ItSignal],
                                                          c_CurOSCSignalCommons[u32_ItSignal],
                                                          c_CurUISignalCommons[u32_ItSignal],
                                                          rc_UiMessage.c_Signals[u32_ItSignal]) != C_NO_ERR)
                           {
                              s32_Retval = C_RANGE;
                           }
                        }
                        //Replace message (LAST)
                        if (c_SyncManager.SetCanMessagePropertiesWithoutDirectionChangeAndWithoutTimeoutChange(
                               c_Id, rc_CurMessage) != C_NO_ERR)
                        {
                           s32_Retval = C_RANGE;
                        }
                     }
                     else
                     {
                        s32_Retval = C_RANGE;
                     }
                  }
                  else
                  {
                     //Replace message
                     if (C_PuiSdHandler::h_GetInstance()->DeleteCanMessage(c_Id) != C_NO_ERR)
                     {
                        s32_Retval = C_RANGE;
                     }
                     else
                     {
                        if (C_PuiSdHandler::h_GetInstance()->InsertCanMessage(c_Id, rc_CurMessage,
                                                                              c_CurOSCSignalCommons,
                                                                              c_CurUISignalCommons,
                                                                              rc_UiMessage,
                                                                              false) != C_NO_ERR)
                        {
                           s32_Retval = C_RANGE;
                        }
                     }
                  }
               }
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}
