//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for export report to DBC file (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QFileInfo>
#include "C_SdUtil.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_OSCLoggingHandler.h"
#include "C_CieExportReportWidget.h"
#include "ui_C_CieExportReportWidget.h"
#include "C_CieExportDbc.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_CieExportReportWidget::mhc_HTML_TABLE_HEADER_START =
   "<td align=\"left\" valign=\"top\" style=\"padding: 5px 18px 5px 0px;white-space:pre;font-weight:bold;\">";
const QString C_CieExportReportWidget::mhc_HTML_TABLE_DATA_START =
   "<td align=\"left\" valign=\"top\" style=\"padding: 5px 18px 5px 0px;white-space:pre;\">";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent          Reference to parent
   \param[in]     orc_FilePath        Loaded file path
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieExportReportWidget::C_CieExportReportWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                 QString & orc_FilePath) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_CieExportReportWidget),
   mrc_ParentDialog(orc_Parent),
   mc_FilePath(orc_FilePath)
{
   const QFileInfo c_FileInfo(orc_FilePath);

   mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // set main title
   this->mrc_ParentDialog.SetTitle(QString(C_GtGetText::h_GetText("%1 File Export")).arg(c_FileInfo.completeSuffix().
                                                                                         toUpper()));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Report"));

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this,
           &C_CieExportReportWidget::m_OkClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieExportReportWidget::~C_CieExportReportWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieExportReportWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelHeadingReport->setText(C_GtGetText::h_GetText("Details"));
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message data for report.

   \param[in] orc_NodeMapping              key is openSYDE name, value is DBC symbol name
   \param[in] orc_ExportStatistic          number of messages and signals of network
   \param[in] orc_Warnings                 warnings of DBC file export

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieExportReportWidget::SetMessageData(const std::map<C_SCLString,
                                                              C_SCLString> & orc_NodeMapping,
                                               const C_CieExportDbc::C_ExportStatistic & orc_ExportStatistic,
                                               const C_SCLStringList & orc_Warnings)
{
   sint32 s32_Return;

   //Copy to internal data
   this->mc_NodeMapping = orc_NodeMapping;
   this->mc_ExportStatistic = orc_ExportStatistic;
   this->mc_Warnings = orc_Warnings;

   // build up report in message dialog field
   s32_Return = m_BuildReport();
   if (s32_Return != C_NO_ERR)
   {
      s32_Return = C_RANGE;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieExportReportWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
/*! \brief   Slot of OK button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieExportReportWidget::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Build up report for DBC export to file.

   \return
   C_NO_ERR Operation success (default)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieExportReportWidget::m_BuildReport(void)
{
   // get nodes
   C_SCLString c_Nodes = "Node(s): ";
   bool q_FirstEntry = true;

   // build up node mapping
   std::map<C_SCLString, C_SCLString>::const_iterator c_Iter;
   for (c_Iter = this->mc_NodeMapping.begin(); c_Iter != this->mc_NodeMapping.end(); ++c_Iter)
   {
      if (q_FirstEntry == true)
      {
         q_FirstEntry = false;
      }
      else
      {
         c_Nodes += ", ";
      }
      c_Nodes += c_Iter->first + " (DBC Name: " + c_Iter->second + ")";
   }

   // build up warning messages
   C_SCLString c_Warnings;
   const uint32 u32_NumOfWarnings = this->mc_Warnings.GetCount();
   if (u32_NumOfWarnings > 0)
   {
      c_Warnings += "<br/>";
      c_Warnings +=  "Warnings: ";
      for (uint32 u32_Pos = 0; u32_Pos < this->mc_Warnings.GetCount(); u32_Pos++)
      {
         c_Warnings += "<br/>";
         c_Warnings += this->mc_Warnings.Strings[u32_Pos];
      }
   }

   QString c_CompleteLog;
   //Start
   c_CompleteLog += "<html>";
   c_CompleteLog += "<body>";
   //File
   c_CompleteLog += "<h3>";
   c_CompleteLog += C_GtGetText::h_GetText("DBC File Information");
   c_CompleteLog += "</h3>";
   c_CompleteLog += "<table><tr>";
   c_CompleteLog += C_CieExportReportWidget::mhc_HTML_TABLE_DATA_START;
   c_CompleteLog += C_GtGetText::h_GetText("Path:");
   c_CompleteLog += "</td>";
   c_CompleteLog += C_CieExportReportWidget::mhc_HTML_TABLE_DATA_START;
   c_CompleteLog += QString("<a href=\"file:%1\"><span style=\"color: %2;\">%3</span></a>").
                    arg(this->mc_FilePath).
                    arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                    arg(this->mc_FilePath);
   c_CompleteLog += "</td></tr><tr>";
   //Content
   c_CompleteLog += C_CieExportReportWidget::mhc_HTML_TABLE_DATA_START;
   c_CompleteLog += C_GtGetText::h_GetText("Content written:");
   c_CompleteLog += "</td>";
   c_CompleteLog += C_CieExportReportWidget::mhc_HTML_TABLE_DATA_START;
   //Add node mapping
   c_CompleteLog += QString(C_GtGetText::h_GetText(c_Nodes.c_str()));
   c_CompleteLog += "<br/>";
   //Translation: 1=Number of Tx messages, 2=Number of Rx messages
   c_CompleteLog += QString(C_GtGetText::h_GetText("Messages: %1;")).arg(
      this->mc_ExportStatistic.u32_NumOfMessages);
   //Translation: 1=Number of signals
   c_CompleteLog += QString(C_GtGetText::h_GetText(" Signals: %1")).arg(
      this->mc_ExportStatistic.u32_NumOfSignals);
   // add warnings
   c_CompleteLog += QString(C_GtGetText::h_GetText(c_Warnings.c_str()));
   c_CompleteLog += "<br/><br/>";
   c_CompleteLog += C_GtGetText::h_GetText("For detailed information see ");
   //Update log file
   C_OSCLoggingHandler::h_Flush();
   c_CompleteLog += QString("<a href=\"file:%1\"><span style=\"color: %2;\">%3</span></a>.").
                    arg(C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str()).
                    arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                    arg(C_GtGetText::h_GetText("log file"));
   c_CompleteLog += "</td></tr></table>";
   //End
   c_CompleteLog += "</body>";
   c_CompleteLog += "</html>";
   this->mpc_Ui->pc_TextBrowserReport->setHtml(c_CompleteLog);

   return C_NO_ERR;
}
