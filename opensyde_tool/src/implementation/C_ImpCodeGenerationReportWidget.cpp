//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Dialog for code generation report (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.10.2018  STW/G.Landsgesell (copied and adapted from data pool list import)
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QFileInfo>
#include "constants.h"
#include "C_GtGetText.h"
#include "C_ImpCodeGenerationReportWidget.h"
#include "ui_C_ImpCodeGenerationReportWidget.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */
const QString C_ImpCodeGenerationReportWidget::mhc_HTML_TABLE_HEADER_START =
   "<td align=\"left\" valign=\"top\" style=\"padding: 5px 18px 5px 0px;white-space:pre;font-weight:bold;\">";
const QString C_ImpCodeGenerationReportWidget::mhc_HTML_TABLE_DATA_START =
   "<td align=\"left\" valign=\"top\" style=\"padding: 5px 18px 5px 0px;white-space:pre;\">";

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent          Reference to parent
   \param[in]     ou32_NodeIndex      Node index
   \param[in]     ou32_DataPoolIndex  Data pool index
   \param[in]     ou32_InterfaceIndex Interface index
   \param[in]     orc_FilePath        Loaded file path

   \created     16.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_ImpCodeGenerationReportWidget::C_ImpCodeGenerationReportWidget(
   stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_ImpCodeGenerationReportWidget),
   mrc_ParentDialog(orc_Parent)
{
   mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // set main title
   this->mrc_ParentDialog.SetTitle(QString(C_GtGetText::h_GetText("Code Generation")));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Report"));

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this,
           &C_ImpCodeGenerationReportWidget::m_OkClicked);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     16.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_ImpCodeGenerationReportWidget::~C_ImpCodeGenerationReportWidget(void)
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     13.04.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_ImpCodeGenerationReportWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelHeadingReport->setText(C_GtGetText::h_GetText("Details"));
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Create report for code generation in HTML.

   \created     16.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_ImpCodeGenerationReportWidget::CreateReport(const std::list<QString> & orc_ExportedFiles) const
{
   QString c_MessageResultText;

   for (std::list<QString>::const_iterator c_It = orc_ExportedFiles.begin();
        c_It != orc_ExportedFiles.end();
        ++c_It)
   {
      QString c_Line = (*c_It);
      c_Line.replace('\\', '/');
      QFileInfo c_FileInfo(c_Line);
      // line is directory -> show full path and add link to file system
      if (c_FileInfo.isDir())
      {
         // first table row:  | Location:| C:\path\to\files |
         c_MessageResultText += mhc_HTML_TABLE_DATA_START;
         c_MessageResultText += C_GtGetText::h_GetText("Location:");
         c_MessageResultText += "</td>";
         c_MessageResultText += mhc_HTML_TABLE_DATA_START;
         c_MessageResultText += QString("<a href=\"file:%1\"><span style=\"color: %2;\">%3</span></a>").
                                arg(c_Line).
                                arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                                arg(c_Line);
         c_MessageResultText += "</td>";
         c_MessageResultText += "</tr>";

         // second table row:  | Generated Files:| something.c + more lines |
         c_MessageResultText += "<tr>";
         c_MessageResultText += mhc_HTML_TABLE_DATA_START;
         c_MessageResultText += C_GtGetText::h_GetText("Generated files:");
         c_MessageResultText += "</td>";
         c_MessageResultText += mhc_HTML_TABLE_DATA_START;
      }
      // line is path to file -> show only file name and add link to file system
      else if (c_FileInfo.exists())
      {
         c_MessageResultText += QString("<a href=\"file:%1\"><span style=\"color: %2;\">%3</span></a>").
                                arg(c_Line).
                                arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                                arg(c_FileInfo.fileName()); // show only files; link will point to actual file
         c_MessageResultText += "<br>";
      }
      // line is section header
      else
      {
         // close previous table
         c_MessageResultText += "</td></tr></table>";
         c_MessageResultText += "<h4>";
         c_MessageResultText += c_Line;
         c_MessageResultText += "</h4>";
         // open new table
         c_MessageResultText += "<table><tr>";
      }
   }

   this->mpc_Ui->pc_TextBrowserReport->setHtml(c_MessageResultText);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information

   \created     16.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_ImpCodeGenerationReportWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of OK button click

   \created     16.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_ImpCodeGenerationReportWidget::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}
