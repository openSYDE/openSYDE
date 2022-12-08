//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for file generation report (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QFileInfo>
#include <QDir>
#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_Uti.hpp"
#include "C_ImpCodeGenerationReportWidget.hpp"
#include "ui_C_ImpCodeGenerationReportWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_ImpCodeGenerationReportWidget::mhc_HTML_TABLE_DATA_START =
   "<td align=\"left\" valign=\"top\" style=\"padding: 2px 18px 2px 2px;white-space:pre;\">";
// the padding order is: top right bottom left

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent    Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_ImpCodeGenerationReportWidget::C_ImpCodeGenerationReportWidget(
   stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_ImpCodeGenerationReportWidget),
   mrc_ParentDialog(orc_Parent)
{
   mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // set main title
   this->mrc_ParentDialog.SetTitle(static_cast<QString>(C_GtGetText::h_GetText("File Generation")));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Report"));

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this,
           &C_ImpCodeGenerationReportWidget::m_OkClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_ImpCodeGenerationReportWidget::~C_ImpCodeGenerationReportWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_ImpCodeGenerationReportWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelHeadingReport->setText(C_GtGetText::h_GetText("Details"));
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create report for file generation in HTML.

   \param[in]  orc_ExportInfo    Information containing exported file paths for each Data Block
*/
//----------------------------------------------------------------------------------------------------------------------
void C_ImpCodeGenerationReportWidget::CreateReport(const std::vector<C_ReportData> & orc_ExportInfo)
const
{
   QString c_MessageResultText;

   for (std::vector<C_ReportData>::const_iterator c_ItDatablocks = orc_ExportInfo.begin();
        c_ItDatablocks != orc_ExportInfo.end(); ++c_ItDatablocks)
   {
      const C_ReportData & rc_ReportData = *c_ItDatablocks;
      const QFileInfo c_DirFileInfo(rc_ReportData.c_CodeGeneratorPath);

      // section title
      c_MessageResultText += "<h4>";
      c_MessageResultText += static_cast<QString>(C_GtGetText::h_GetText("\n%1, Data Block \"%2\"")).
                             arg(rc_ReportData.c_NodeName).arg(rc_ReportData.c_AppName);
      c_MessageResultText += "</h4>";

      // open new table
      c_MessageResultText += "<table><tr>";

      // first table row:  | File Generator: | C:/path/to/syde_coder.exe |
      c_MessageResultText += mhc_HTML_TABLE_DATA_START;
      c_MessageResultText += C_GtGetText::h_GetText("File Generator:");
      c_MessageResultText += "</td>";
      c_MessageResultText += mhc_HTML_TABLE_DATA_START;
      c_MessageResultText +=
         C_Uti::h_GetLink(rc_ReportData.c_CodeGeneratorPath, mc_STYLE_GUIDE_COLOR_LINK,
                          c_DirFileInfo.absoluteDir().absolutePath()); // use directory for link target location
      c_MessageResultText += "</td>";
      c_MessageResultText += "</tr>";

      // second table row:  | File Structure Version: | 3 |
      c_MessageResultText += "<tr>";
      c_MessageResultText += mhc_HTML_TABLE_DATA_START;
      c_MessageResultText += C_GtGetText::h_GetText("File Structure:");
      c_MessageResultText += "</td>";
      c_MessageResultText += mhc_HTML_TABLE_DATA_START;
      c_MessageResultText += C_GtGetText::h_GetText("Version ");
      c_MessageResultText += QString::number(rc_ReportData.u16_CodeVersion);
      c_MessageResultText += "</td>";
      c_MessageResultText += "</tr>";

      // third table row:  | Location:| C:/path/to/files |
      c_MessageResultText += "<tr>";
      c_MessageResultText += mhc_HTML_TABLE_DATA_START;
      c_MessageResultText += C_GtGetText::h_GetText("Location:");
      c_MessageResultText += "</td>";
      c_MessageResultText += mhc_HTML_TABLE_DATA_START;
      c_MessageResultText += C_Uti::h_GetLink(rc_ReportData.c_Directory, mc_STYLE_GUIDE_COLOR_LINK,
                                              rc_ReportData.c_Directory);
      c_MessageResultText += "</td>";
      c_MessageResultText += "</tr>";

      // fourth table row:  | Generated Files:| my_datapool.c (one row for each generated file) |
      c_MessageResultText += "<tr>";
      c_MessageResultText += mhc_HTML_TABLE_DATA_START;
      c_MessageResultText += C_GtGetText::h_GetText("Generated Files:");
      c_MessageResultText += "</td>";
      c_MessageResultText += mhc_HTML_TABLE_DATA_START;

      // add generated files to table
      for (QStringList::ConstIterator c_ItFiles = rc_ReportData.c_GeneratedFiles.begin();
           c_ItFiles != rc_ReportData.c_GeneratedFiles.end(); ++c_ItFiles)
      {
         const QFileInfo c_FileInfo(*c_ItFiles);
         // show only file names; link will point to actual file
         c_MessageResultText += C_Uti::h_GetLink(c_FileInfo.fileName(), mc_STYLE_GUIDE_COLOR_LINK,
                                                 c_FileInfo.filePath());
         c_MessageResultText += "<br>";
      }

      // add extra new line if file list is empty
      if (rc_ReportData.c_GeneratedFiles.isEmpty() == true)
      {
         c_MessageResultText += "<br>";
      }

      // close table
      c_MessageResultText += "</td></tr></table>";
   }

   this->mpc_Ui->pc_TextBrowserReport->setHtml(c_MessageResultText);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_ImpCodeGenerationReportWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
/*! \brief   Slot of OK button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_ImpCodeGenerationReportWidget::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}
