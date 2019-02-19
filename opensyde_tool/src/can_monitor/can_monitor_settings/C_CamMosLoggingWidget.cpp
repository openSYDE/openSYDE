//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for configuring file logging.
 (implementation)

   Widget for configuring file logging.


   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.11.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QDir>
#include <QCompleter>
#include <QDirModel>
#include <QScrollBar>
#include <QStyledItemDelegate>
#include <QFileDialog>
#include <QDateTime>

#include "C_CamMosLoggingWidget.h"
#include "ui_C_CamMosLoggingWidget.h"

#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "C_UsHandler.h"
#include "C_CamProHandler.h"
#include "C_Uti.h"
#include "C_OgeWiCustomMessage.h"
#include "stwerrors.h"
#include "C_CamUti.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_types;
/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     15.11.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_CamMosLoggingWidget::C_CamMosLoggingWidget(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_CamMosLoggingWidget),
   mq_Online(false)
{
   QCompleter * pc_Completer;
   QStyledItemDelegate * pc_ItemDelegate;

   this->mpc_Ui->setupUi(this);

   // initialize background color
   this->SetBackgroundColor(5);

   // initialize title widget
   this->mpc_Ui->pc_WiHeader->SetTitle(C_GtGetText::h_GetText("Logging"));
   this->mpc_Ui->pc_WiHeader->SetIcon("://images/IconLogging.svg");
   this->mpc_Ui->pc_WiHeader->SetToggle(true);
   this->mpc_Ui->pc_WiHeader->SetToggleToolTip(C_GtGetText::h_GetText("Logging"),
                                               C_GtGetText::h_GetText("Start or stop logging to file."));

   // initialize labels
   this->mpc_Ui->pc_LabFile->SetForegroundColor(0);
   this->mpc_Ui->pc_LabFolder->SetForegroundColor(0);
   this->mpc_Ui->pc_LabFormat->SetForegroundColor(0);
   this->mpc_Ui->pc_LabOverwrite->SetForegroundColor(0);
   this->mpc_Ui->pc_LabFile->SetFontPixel(13);
   this->mpc_Ui->pc_LabFolder->SetFontPixel(13);
   this->mpc_Ui->pc_LabFormat->SetFontPixel(13);
   this->mpc_Ui->pc_LabOverwrite->SetFontPixel(13);

   this->mpc_Ui->pc_LabFile->setText(C_GtGetText::h_GetText("File"));
   this->mpc_Ui->pc_LabFile->SetToolTipInformation(C_GtGetText::h_GetText("File"),
                                                   C_GtGetText::h_GetText("Name of log file without extension."));
   this->mpc_Ui->pc_LabFolder->setText(C_GtGetText::h_GetText("Directory"));
   this->mpc_Ui->pc_LabFolder->SetToolTipInformation(C_GtGetText::h_GetText("Directory"),
                                                     C_GtGetText::h_GetText(
                                                        "Path of directory where log file(s) get saved at."));
   this->mpc_Ui->pc_LabFormat->setText(C_GtGetText::h_GetText("Format"));
   this->mpc_Ui->pc_LabFormat->SetToolTipInformation(C_GtGetText::h_GetText("Format"),
                                                     C_GtGetText::h_GetText("File format of log file (*.asc or *.blf)."));
   this->mpc_Ui->pc_LabOverwrite->setText(C_GtGetText::h_GetText("File Options"));
   this->mpc_Ui->pc_LabOverwrite->SetToolTipInformation(
      C_GtGetText::h_GetText("File Options"),
      C_GtGetText::h_GetText("File overwrite options:\n"
                             "  - Make log file name unique by adding a timestamp suffix. Format: <LogFileName>_<Date>_<Time>\n"
                             "  - Overwrite an existing file but warn before doing so.\n"
                             "  - Overwrite an existing file without warning."));

   //Auto completion
   pc_Completer = new QCompleter(this);
   pc_Completer->setModel(new QDirModel(QStringList(), QDir::AllDirs | QDir::Drives | QDir::NoDotAndDotDot, QDir::Name,
                                        pc_Completer));
   //this code allows to handle the QListView::item in stylesheets
   pc_ItemDelegate = new QStyledItemDelegate();

   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(pc_Completer->popup(), "View", "CompleterDark");
   pc_Completer->popup()->setItemDelegate(pc_ItemDelegate);

   this->mpc_Ui->pc_LeFolder->setCompleter(pc_Completer);

   // initialize combo boxes
   this->mpc_Ui->pc_CbxFormat->addItem("ASC");
   this->mpc_Ui->pc_CbxFormat->addItem("BLF");

   this->mpc_Ui->pc_CbxOverwrite->addItem(C_GtGetText::h_GetText("Add timestamp suffix"));
   this->mpc_Ui->pc_CbxOverwrite->addItem(C_GtGetText::h_GetText("Warn before overwriting"));
   this->mpc_Ui->pc_CbxOverwrite->addItem(C_GtGetText::h_GetText("Overwrite without warning"));

   // initialize button
   this->mpc_Ui->pc_PubBrowse->setText("");

   // connections
   connect(this->mpc_Ui->pc_WiHeader, &C_CamOgeWiSettingSubSection::SigExpandSection,
           this, &C_CamMosLoggingWidget::m_OnExpand);
   connect(this->mpc_Ui->pc_WiHeader, &C_CamOgeWiSettingSubSection::SigToggled, this,
           &C_CamMosLoggingWidget::m_OnToggled);
   connect(C_CamProHandler::h_GetInstance(), &C_CamProHandler::SigNewConfiguration,
           this, &C_CamMosLoggingWidget::m_LoadConfig);
   connect(this->mpc_Ui->pc_LeFolder, &C_CamOgeLeDarkBrowse::editingFinished,
           this, &C_CamMosLoggingWidget::m_OnFolderEdited);
   connect(this->mpc_Ui->pc_LeFile, &C_CamOgeLeDarkBrowse::editingFinished,
           this, &C_CamMosLoggingWidget::m_OnFileNameEdited);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_CbxOverwrite, static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged),
           this, &C_CamMosLoggingWidget::m_OnOverwriteModeSelected);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_CbxFormat, static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged),
           this, &C_CamMosLoggingWidget::m_OnFormatSelected);
   connect(this->mpc_Ui->pc_PubBrowse, &C_CamOgePubDarkBrowse::clicked, this, &C_CamMosLoggingWidget::m_OnBrowse);

   //lint -e{429}  no memory leak because of the parent of pc_Completer & pc_ItemDelegate and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     15.11.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_CamMosLoggingWidget::~C_CamMosLoggingWidget()
{
   delete this->mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load all user settings.

   \created     21.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosLoggingWidget::LoadUserSettings() const
{
   this->mpc_Ui->pc_WiHeader->SetExpanded(C_UsHandler::h_GetInstance()->GetWiLoggingExpanded());
   this->m_OnExpand(C_UsHandler::h_GetInstance()->GetWiLoggingExpanded());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Prepare widget for menu view in collapsed state resp. expanded view.

   \param[in]     oq_Expand       true: prepare for expanded, false: prepare for collapsed menu

   \created     14.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosLoggingWidget::PrepareForExpanded(const bool oq_Expand) const
{
   if (oq_Expand == false)
   {
      this->mpc_Ui->pc_WiContent->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_WiContent->setVisible(C_UsHandler::h_GetInstance()->GetWiLoggingExpanded());
   }

   // hide arrow button
   this->mpc_Ui->pc_WiHeader->ShowExpandButton(oq_Expand);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot for communication change signal.

   \param[in]  oq_Online    true: communication started; false: communication stopped.

   \created     17.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosLoggingWidget::OnCommunicationStarted(const bool oq_Online)
{
   this->mq_Online = oq_Online;
   this->m_DecideStartStop();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot to forward log file add result.

   \param[in]   os32_Result   result of log file add operation

   \created     17.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosLoggingWidget::OnSigLogFileAddResult(const sint32 os32_Result)
{
   if (os32_Result != stw_errors::C_NO_ERR)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Start logging"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Could not start logging. Possible reason: Missing write access."));
      c_Message.Execute();
      this->m_OnToggled(false);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger new log file location (relative to project).

   \created     05.02.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosLoggingWidget::OnSigSavedAsNew(void) const
{
   this->m_OnFolderEdited();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load configuration.

   \created     16.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_LoadConfig(void) const
{
   const C_CamProLoggingData c_LoggingData = C_CamProHandler::h_GetInstance()->GetLoggingData();

   // use default logging directory
   if (c_LoggingData.c_Directory == "")
   {
      QString c_DefaultDir = "./CANlogs";
      C_CamProHandler::h_GetInstance()->SetLoggingDirectory(c_DefaultDir);
      this->mpc_Ui->pc_LeFolder->SetPath(c_DefaultDir);
      this->mpc_Ui->pc_LeFolder->SetToolTipInformation("", C_CamUti::h_GetAbsPathFromProj(c_DefaultDir));
   }
   else
   {
      this->mpc_Ui->pc_LeFolder->SetPath(c_LoggingData.c_Directory);
      this->mpc_Ui->pc_LeFolder->SetToolTipInformation("", C_CamUti::h_GetAbsPathFromProj(c_LoggingData.c_Directory));
   }

   // remaining values
   this->mpc_Ui->pc_LeFile->setText(c_LoggingData.c_FileName);
   this->mpc_Ui->pc_CbxOverwrite->setCurrentIndex(static_cast<sintn>(c_LoggingData.e_OverwriteMode));
   this->mpc_Ui->pc_CbxFormat->setCurrentIndex(static_cast<sintn>(c_LoggingData.e_FileFormat));
   this->mpc_Ui->pc_WiHeader->SetToggleState(c_LoggingData.q_Enabled);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of expand signal.

   \param[in]  oq_Expand    true: expand; false: collapse

   \created     10.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_OnExpand(const bool oq_Expand) const
{
   this->mpc_Ui->pc_WiContent->setVisible(oq_Expand);
   C_UsHandler::h_GetInstance()->SetWiLoggingExpanded(oq_Expand);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Enable/disable whole logging widget.

   Slot of toggle toggled.

   \created     16.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_OnToggled(const bool oq_Enabled)
{
   // update data handling
   C_CamProHandler::h_GetInstance()->SetLoggingEnabled(oq_Enabled);

   // start or stop
   this->m_DecideStartStop();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot for folder line edit editing finished signal.

   \created     16.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_OnFolderEdited(void) const
{
   const QString c_Path = this->mpc_Ui->pc_LeFolder->GetPath();

   // update data handling
   C_CamProHandler::h_GetInstance()->SetLoggingDirectory(c_Path);

   // update tool tip
   this->mpc_Ui->pc_LeFolder->SetToolTipInformation("", C_CamUti::h_GetAbsPathFromProj(c_Path));
}

//-----------------------------------------------------------------------------
/*!
   brief   Slot for file name line edit editing finished signal.

   \created     16.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_OnFileNameEdited(void) const
{
   // update data handling
   C_CamProHandler::h_GetInstance()->SetLoggingFileName(this->mpc_Ui->pc_LeFile->text());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot for file options combo box index changed.

   \param[in]     os32_Index        new combo box index

   \created     16.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_OnOverwriteModeSelected(const sint32 os32_Index) const
{
   // update data handling
   C_CamProHandler::h_GetInstance()->SetLoggingOverwriteMode(
      static_cast<C_CamProLoggingData::E_OverwriteMode>(os32_Index));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot for file format combo box index changed.

   \param[in]     os32_Index        new combo box index

   \created     16.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_OnFormatSelected(const sint32 os32_Index) const
{
   // update data handling
   C_CamProHandler::h_GetInstance()->SetLoggingFormat(static_cast<C_CamProLoggingData::E_Format>(os32_Index));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot for browse button click.

   Browse for directory.

   \created     16.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_OnBrowse(void)
{
   QString c_Path;
   QString c_Folder = C_CamUti::h_GetAbsPathFromProj(C_CamProHandler::h_GetInstance()->GetLoggingData().c_Directory);

   if (c_Folder == "")
   {
      // use project path if no directory is given
      c_Folder = C_CamProHandler::h_GetInstance()->GetCurrentProjDir();

      if (c_Folder == "")
      {
         // fallback: exe path
         c_Folder = C_Uti::h_GetExePath();
      }
   }

   c_Path = QFileDialog::getExistingDirectory(this, C_GtGetText::h_GetText("Select Log File Location"),
                                              c_Folder, QFileDialog::ShowDirsOnly);

   if (c_Path != "")
   {
      c_Path =
         C_CamUti::h_AskUserToSaveRelativePath(this, c_Path, C_CamProHandler::h_GetInstance()->GetCurrentProjDir());
      this->mpc_Ui->pc_LeFolder->SetPath(c_Path);
      m_OnFolderEdited();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Use online flag and enabled flag to decide if to start or stop logging.

   \created     17.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_DecideStartStop()
{
   if ((this->mq_Online == true) && (C_CamProHandler::h_GetInstance()->GetLoggingData().q_Enabled == true))
   {
      this->mpc_Ui->pc_WiContent->setEnabled(false);
      this->mpc_Ui->pc_WiHeader->SetTitle(C_GtGetText::h_GetText("Logging (active)"));

      // check and start logging
      this->m_CheckAndStartLogging();
   }
   else
   {
      this->mpc_Ui->pc_WiHeader->SetTitle(C_GtGetText::h_GetText("Logging"));
      this->mpc_Ui->pc_WiContent->setEnabled(true);

      // stop logging
      //GUI only supports one log file at once so this is also used for removing a log file.
      Q_EMIT (this->SigRemoveAllLogFiles());
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check path and flags and start logging.

   \created     16.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_CheckAndStartLogging()
{
   const C_CamProLoggingData c_LoggingData = C_CamProHandler::h_GetInstance()->GetLoggingData();
   QFileInfo c_FileInfo;
   bool q_Continue = true;

   // glue together file path
   QString c_FilePath = C_CamUti::h_GetAbsPathFromProj(c_LoggingData.c_Directory) + "/" + c_LoggingData.c_FileName;

   // eventually add timestamp
   if (c_LoggingData.e_OverwriteMode == C_CamProLoggingData::eADD_TIMESTAMP)
   {
      QDateTime c_Time = QDateTime::currentDateTime();
      c_FilePath += c_Time.toString("__yyyy_MM_dd__HH_mm_ss");
   }

   // add suffix
   c_FilePath += "." + C_CamProLoggingData::h_FormatEnumToString(c_LoggingData.e_FileFormat);

   // check existence if flag is warn-before-overwrite (in the "fast-click" case that a file with timestamp suffix
   // already exists we also just overwrite because the first file would not hold much information)
   c_FileInfo.setFile(c_FilePath);
   if (c_LoggingData.e_OverwriteMode == C_CamProLoggingData::eWARN_BEFORE_OVERWRITE)
   {
      if (c_FileInfo.exists() == true)
      {
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eWARNING);
         c_Message.SetHeading(C_GtGetText::h_GetText("Log file overwrite"));
         c_Message.SetDescription(QString(C_GtGetText::h_GetText("The log file %1 already exists. "
                                                                 "Do you want to overwrite the file?")).
                                  arg(c_FileInfo.absoluteFilePath()));
         c_Message.SetCancelButtonText(C_GtGetText::h_GetText("Cancel"));
         c_Message.SetOKButtonText(C_GtGetText::h_GetText("Overwrite"));
         q_Continue = (c_Message.Execute() == C_OgeWiCustomMessage::eOK);
      }
   }

   if (q_Continue == true)
   {
      // start logging
      if (c_LoggingData.e_FileFormat == C_CamProLoggingData::eBLF)
      {
         Q_EMIT (this->SigAddLogFileBlf(c_FileInfo.absoluteFilePath()));
      }
      else
      {
         Q_EMIT (this->SigAddLogFileAsc(c_FileInfo.absoluteFilePath()));
      }
   }
   else
   {
      // set logging off
      this->mpc_Ui->pc_WiHeader->SetToggleState(false);
      this->m_OnToggled(false);
   }
}
