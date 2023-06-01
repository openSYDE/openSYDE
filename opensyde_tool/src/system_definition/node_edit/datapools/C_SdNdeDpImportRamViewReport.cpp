//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup dialog widget to show the report of an RAMView project import.

   Popup dialog widget to show the report of an RAMView project import.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GtGetText.hpp"
#include "constants.hpp"
#include "C_Uti.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_OscLoggingHandler.hpp"

#include "C_SdNdeDpImportRamViewReport.hpp"
#include "ui_C_SdNdeDpImportRamViewReport.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
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

   \param[in,out]  orc_Parent             Reference to parent
   \param[in]      orc_ProjectPath        RAMView project file path (*.def file)
   \param[in]      orc_DataPool           Imported core Datapool
   \param[in]      orc_ImportInformation  List of import information (warnings, adaptions, ...)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpImportRamViewReport::C_SdNdeDpImportRamViewReport(C_OgePopUpDialog & orc_Parent,
                                                           const QString & orc_ProjectPath,
                                                           const C_OscNodeDataPool & orc_DataPool,
                                                           const C_SclStringList & orc_ImportInformation) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeDpImportRamViewReport),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   this->m_FillReport(orc_ProjectPath, orc_DataPool, orc_ImportInformation);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SdNdeDpImportRamViewReport::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked,
           this, &C_SdNdeDpImportRamViewReport::m_CancelClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpImportRamViewReport::~C_SdNdeDpImportRamViewReport(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpImportRamViewReport::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("RAMView Project Import"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Report"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Details"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Continue"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpImportRamViewReport::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
void C_SdNdeDpImportRamViewReport::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpImportRamViewReport::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill report with information from RAMView project import

   \param[in]  orc_ProjectPath         RAMView project file path (*.def file)
   \param[in]  orc_DataPool            Imported core Datapool
   \param[in]  orc_ImportInformation   List of import information (warnings, adaptions, ...)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpImportRamViewReport::m_FillReport(const QString & orc_ProjectPath, const C_OscNodeDataPool & orc_DataPool,
                                                const C_SclStringList & orc_ImportInformation)
{
   QString c_Text = "";

   const QString c_HtmlTableDataStart =
      "<td align=\"left\" valign=\"top\" style=\"padding: 5px 18px 5px 0px;white-space:pre;\">";
   const QString c_RamOrEeprom = (orc_DataPool.e_Type == C_OscNodeDataPool::eNVM) ? "EEPROM" : "RAM";

   // Count all Datapool elements
   uint32_t u32_ElementCount = 0;

   for (std::vector<C_OscNodeDataPoolList>::const_iterator c_ItLists = orc_DataPool.c_Lists.begin();
        c_ItLists < orc_DataPool.c_Lists.end(); ++c_ItLists)
   {
      u32_ElementCount += c_ItLists->c_Elements.size();
   }

   // Source File Information
   c_Text += "<h3>";

   c_Text += C_GtGetText::h_GetText("Source File Information");
   c_Text += "</h3>";
   c_Text += "<table><tr>";
   c_Text += c_HtmlTableDataStart;
   c_Text += C_GtGetText::h_GetText("Path:");
   c_Text += "</td>";
   c_Text += c_HtmlTableDataStart;
   c_Text += C_Uti::h_GetLink(orc_ProjectPath, mc_STYLE_GUIDE_COLOR_LINK, orc_ProjectPath);
   c_Text += "</td></tr><tr>";
   c_Text += c_HtmlTableDataStart;
   c_Text += C_GtGetText::h_GetText("Read Content: ");
   c_Text += "</td>";
   c_Text += c_HtmlTableDataStart;
   c_Text += static_cast<QString>(C_GtGetText::h_GetText("Found %1 %2 Datapool list(s) containing %3 element(s) that "
                                                         "will be added to a new %4 Datapool.")).
             arg(orc_DataPool.c_Lists.size()).
             arg(c_RamOrEeprom).
             arg(u32_ElementCount).
             arg(C_PuiSdUtil::h_ConvertDataPoolTypeToString(orc_DataPool.e_Type));
   c_Text += "<br/>";
   c_Text += C_GtGetText::h_GetText("For parsing errors, warnings and detailed information see ");
   c_Text += C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"), mc_STYLE_GUIDE_COLOR_LINK,
                              C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str());
   c_Text += ".";
   c_Text += "</td></tr></table>";

   // Import information (warnings, adaptions, ...)
   c_Text += "<h3>";
   c_Text += C_GtGetText::h_GetText("Import Information");
   c_Text += "</h3><p>";
   c_Text += C_GtGetText::h_GetText("General: Datapool names, list names and element names are adapted as follows:");
   c_Text += "<br/>";
   c_Text += "- ";
   c_Text += C_GtGetText::h_GetText("Eliminate spaces");
   c_Text += "<br/>";
   c_Text += "- ";
   c_Text += C_GtGetText::h_GetText("Cut to 31 characters");
   c_Text += "</p>";
   if (orc_ImportInformation.GetCount() > 0)
   {
      c_Text += "<p><b>";
      c_Text += C_GtGetText::h_GetText("Import Details");
      c_Text += "</b></p>";

      // List with "-" as bullet points is not supported by Qt's rich text engine, so we use a table as workaround
      c_Text += "<table>";
      for (uint32_t u32_Info = 0; u32_Info < orc_ImportInformation.GetCount(); u32_Info++)
      {
         c_Text += "<tr><td style=\"padding-right:2px\">-</td><td>";
         c_Text += orc_ImportInformation.Strings[u32_Info].c_str();
         c_Text += "</td></tr>";
      }
      c_Text += "</table>";
   }

   C_OscLoggingHandler::h_Flush(); // update log file
   this->mpc_Ui->pc_TextBrowser->setHtml(c_Text);
}
