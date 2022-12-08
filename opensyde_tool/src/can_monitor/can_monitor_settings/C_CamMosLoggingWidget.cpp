//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for configuring file logging.
 (implementation)

   Widget for configuring file logging.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QDir>
#include <QDirModel>
#include <QScrollBar>
#include <QFileDialog>
#include <QDateTime>

#include "C_CamMosLoggingWidget.hpp"
#include "ui_C_CamMosLoggingWidget.h"

#include "C_OscUtils.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_UsHandler.hpp"
#include "C_CamProHandler.hpp"
#include "C_Uti.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "stwerrors.hpp"
#include "C_CamUti.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosLoggingWidget::C_CamMosLoggingWidget(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_CamMosLoggingWidget),
   mq_Online(false)
{
   this->mpc_Ui->setupUi(this);

   // Activate drag and drop for path line edit
   this->mpc_Ui->pc_LeFolder->SetDragAndDropActiveForFolder(true);

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

   // initialize combo boxes
   this->mpc_Ui->pc_CbxFormat->addItem("ASC");
   this->mpc_Ui->pc_CbxFormat->addItem("BLF");

   this->mpc_Ui->pc_CbxOverwrite->addItem(C_GtGetText::h_GetText("Add timestamp suffix"));
   this->mpc_Ui->pc_CbxOverwrite->addItem(C_GtGetText::h_GetText("Warn before overwriting"));
   this->mpc_Ui->pc_CbxOverwrite->addItem(C_GtGetText::h_GetText("Overwrite without warning"));

   // initialize button
   this->mpc_Ui->pc_PubBrowse->setText("");
   this->mpc_Ui->pc_PubVariables->setText("");

   // connections
   connect(this->mpc_Ui->pc_WiHeader, &C_CamOgeWiSettingSubSection::SigExpandSection,
           this, &C_CamMosLoggingWidget::m_OnExpand);
   connect(this->mpc_Ui->pc_WiHeader, &C_CamOgeWiSettingSubSection::SigHide,
           this, &C_CamMosLoggingWidget::SigHide);
   connect(this->mpc_Ui->pc_WiHeader, &C_CamOgeWiSettingSubSection::SigToggled, this,
           &C_CamMosLoggingWidget::m_OnToggled);
   connect(C_CamProHandler::h_GetInstance(), &C_CamProHandler::SigNewConfiguration,
           this, &C_CamMosLoggingWidget::m_LoadConfig);
   connect(this->mpc_Ui->pc_LeFolder, &C_CamOgeLeFilePath::editingFinished,
           this, &C_CamMosLoggingWidget::m_OnFolderEdited);
   connect(this->mpc_Ui->pc_LeFolder, &C_CamOgeLeFilePath::SigPathDropped,
           this, &C_CamMosLoggingWidget::m_OnDroppedPath);
   connect(this->mpc_Ui->pc_LeFile, &C_OgeLeDark::editingFinished, this, &C_CamMosLoggingWidget::m_OnFileNameEdited);
   connect(this->mpc_Ui->pc_CbxOverwrite, static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
           this, &C_CamMosLoggingWidget::m_OnOverwriteModeSelected);
   connect(this->mpc_Ui->pc_CbxFormat, static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
           this, &C_CamMosLoggingWidget::m_OnFormatSelected);
   connect(this->mpc_Ui->pc_PubBrowse, &C_CamOgePubDarkBrowse::clicked, this, &C_CamMosLoggingWidget::m_OnBrowse);
   connect(this->mpc_Ui->pc_PubVariables, &C_CamOgePubPathVariables::SigVariableSelected,
           this, &C_CamMosLoggingWidget::m_InsertPathVar);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosLoggingWidget::~C_CamMosLoggingWidget(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load all user settings.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::LoadUserSettings(void) const
{
   this->mpc_Ui->pc_WiHeader->SetExpanded(C_UsHandler::h_GetInstance()->GetWiLoggingExpanded());
   this->m_OnExpand(C_UsHandler::h_GetInstance()->GetWiLoggingExpanded());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare widget for menu view in collapsed state resp. expanded view.

   \param[in]     oq_Expand       true: prepare for expanded, false: prepare for collapsed menu
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for communication change signal.

   \param[in]  oq_Online    true: communication started; false: communication stopped.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::OnCommunicationStarted(const bool oq_Online)
{
   this->mq_Online = oq_Online;
   this->m_DecideStartStop();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot to forward log file add result.

   \param[in]   os32_Result   result of log file add operation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::OnSigLogFileAddResult(const int32_t os32_Result)
{
   if (os32_Result != stw::errors::C_NO_ERR)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Start logging"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Could not start logging. Possible reason: Missing write access."));
      c_Message.Execute();
      this->m_OnToggled(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger new log file location (relative to project).
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::OnSigSavedAsNew(void)
{
   this->m_OnFolderEdited();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load configuration.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_LoadConfig(void) const
{
   const C_CamProLoggingData & rc_LoggingData = C_CamProHandler::h_GetInstance()->GetLoggingData();

   this->m_LoadFolderConfig();

   // remaining values
   this->mpc_Ui->pc_LeFile->setText(rc_LoggingData.c_FileName);
   this->mpc_Ui->pc_CbxOverwrite->setCurrentIndex(static_cast<int32_t>(rc_LoggingData.e_OverwriteMode));
   this->mpc_Ui->pc_CbxFormat->setCurrentIndex(static_cast<int32_t>(rc_LoggingData.e_FileFormat));
   this->mpc_Ui->pc_WiHeader->SetToggleState(rc_LoggingData.q_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load folder configuration.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_LoadFolderConfig(void) const
{
   const C_CamProLoggingData & rc_LoggingData = C_CamProHandler::h_GetInstance()->GetLoggingData();

   // use default logging directory
   if (rc_LoggingData.c_Directory == "")
   {
      const QString c_DefaultDir = "./CANlogs";
      C_CamProHandler::h_GetInstance()->SetLoggingDirectory(c_DefaultDir);
      this->mpc_Ui->pc_LeFolder->SetPath(c_DefaultDir, C_CamProHandler::h_GetInstance()->GetCurrentProjDir());
   }
   else
   {
      this->mpc_Ui->pc_LeFolder->SetPath(rc_LoggingData.c_Directory,
                                         C_CamProHandler::h_GetInstance()->GetCurrentProjDir());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of expand signal.

   \param[in]  oq_Expand    true: expand; false: collapse
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_OnExpand(const bool oq_Expand) const
{
   this->mpc_Ui->pc_WiContent->setVisible(oq_Expand);
   C_UsHandler::h_GetInstance()->SetWiLoggingExpanded(oq_Expand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enable/disable whole logging widget.

   Slot of toggle toggled.

   \param[in]  oq_Enabled  enabled/disabled flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_OnToggled(const bool oq_Enabled)
{
   // update data handling
   C_CamProHandler::h_GetInstance()->SetLoggingEnabled(oq_Enabled);

   // start or stop
   this->m_DecideStartStop();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for folder line edit editing finished signal.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_OnFolderEdited(void)
{
   // update data handling
   C_CamProHandler::h_GetInstance()->SetLoggingDirectory(this->mpc_Ui->pc_LeFolder->GetPath());
}

//----------------------------------------------------------------------------------------------------------------------
/*!
   brief   Slot for file name line edit editing finished signal.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_OnFileNameEdited(void)
{
   // update data handling
   C_CamProHandler::h_GetInstance()->SetLoggingFileName(this->mpc_Ui->pc_LeFile->text());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for file options combo box index changed.

   \param[in]     os32_Index        new combo box index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_OnOverwriteModeSelected(const int32_t os32_Index) const
{
   // update data handling
   C_CamProHandler::h_GetInstance()->SetLoggingOverwriteMode(
      static_cast<C_CamProLoggingData::E_OverwriteMode>(os32_Index));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for file format combo box index changed.

   \param[in]     os32_Index        new combo box index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_OnFormatSelected(const int32_t os32_Index) const
{
   // update data handling
   C_CamProHandler::h_GetInstance()->SetLoggingFormat(static_cast<C_CamProLoggingData::E_Format>(os32_Index));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for browse button click.

   Browse for directory.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_OnBrowse(void)
{
   QString c_Path;
   QString c_Folder =
      C_CamUti::h_GetResolvedAbsolutePathFromProj(C_CamProHandler::h_GetInstance()->GetLoggingData().c_Directory);

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
      this->m_SetLoggingPath(c_Path);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle a dropped folder path in create in line edit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_OnDroppedPath(void)
{
   this->m_SetLoggingPath(this->mpc_Ui->pc_LeFolder->GetPath());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setter for full create in path.

   \param[in] orc_New New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_SetLoggingPath(const QString & orc_New)
{
   const QString c_Path =
      C_CamUti::h_AskUserToSaveRelativePath(this, orc_New, C_CamProHandler::h_GetInstance()->GetCurrentProjDir());

   // if path contains invalid characters this returned empty
   if (c_Path.isEmpty() == false)
   {
      this->mpc_Ui->pc_LeFolder->SetPath(c_Path, C_CamProHandler::h_GetInstance()->GetCurrentProjDir());
      this->m_OnFolderEdited();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Insert selected path variable in log directory path string.

   \param[in]       orc_Variable     path variable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_InsertPathVar(const QString & orc_Variable)
{
   this->mpc_Ui->pc_LeFolder->InsertVariable(orc_Variable);
   this->m_OnFolderEdited();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Use online flag and enabled flag to decide if to start or stop logging.
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check path and flags and start logging.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosLoggingWidget::m_CheckAndStartLogging()
{
   const C_CamProLoggingData & rc_LoggingData = C_CamProHandler::h_GetInstance()->GetLoggingData();
   const QString c_ResolvedPath =
      C_CamUti::h_ResolvePlaceholderVariables(rc_LoggingData.c_Directory.toStdString().c_str());
   QFileInfo c_FileInfo;
   bool q_ValidDir;
   bool q_ValidName;
   bool q_Continue = true;

   // check directory
   q_ValidDir = stw::opensyde_core::C_OscUtils::h_CheckValidFilePath(c_ResolvedPath.toStdString().c_str());

   // check file name
   q_ValidName = stw::opensyde_core::C_OscUtils::h_CheckValidFileName(rc_LoggingData.c_FileName.toStdString().c_str());

   if ((q_ValidDir == false) || (q_ValidName == false))
   {
      // inform user
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      const QString c_Heading = C_GtGetText::h_GetText("Logging Directory");
      const QString c_MessageText =
         C_GtGetText::h_GetText("Could not start logging because of invalid log file name or directory. "
                                "For more information see details.");
      QString c_Details = "";

      if (q_ValidDir == false)
      {
         if (rc_LoggingData.c_Directory.isEmpty() == false)
         {
            c_Details += C_GtGetText::h_GetText("The directory contains invalid characters:\n");
            c_Details += rc_LoggingData.c_Directory;
            if (rc_LoggingData.c_Directory != c_ResolvedPath)
            {
               c_Details += C_GtGetText::h_GetText(" (resolved: ") + c_ResolvedPath + ")";
            }
            c_Details += "\n";
         }
         else
         {
            c_Details += C_GtGetText::h_GetText("The directory path is empty.\n");
         }
      }

      if (q_ValidName == false)
      {
         if (rc_LoggingData.c_FileName.isEmpty() == false)
         {
            c_Details += C_GtGetText::h_GetText("The file name contains invalid characters:\n");
            c_Details += rc_LoggingData.c_FileName;
         }
         else
         {
            c_Details += C_GtGetText::h_GetText("The file name is empty.");
         }
      }

      c_Message.SetHeading(c_Heading);
      c_Message.SetDescription(c_MessageText);
      c_Message.SetDetails(c_Details);
      c_Message.Execute();

      // Abort logging start
      q_Continue = false;
   }

   if (q_Continue == true)
   {
      QString c_FilePath;

      // glue together file path
      c_FilePath =
         C_CamUti::h_GetResolvedAbsolutePathFromProj(rc_LoggingData.c_Directory) + "/" + rc_LoggingData.c_FileName;

      // eventually add timestamp
      if (rc_LoggingData.e_OverwriteMode == C_CamProLoggingData::eADD_TIMESTAMP)
      {
         const QDateTime c_Time = QDateTime::currentDateTime();
         c_FilePath += c_Time.toString("__yyyy_MM_dd__HH_mm_ss");
      }

      // add suffix
      c_FilePath += "." + C_CamProLoggingData::h_FormatEnumToString(rc_LoggingData.e_FileFormat);

      // check existence if flag is warn-before-overwrite (in the "fast-click" case that a file with timestamp suffix
      // already exists we also just overwrite because the first file would not hold much information)
      c_FileInfo.setFile(c_FilePath);
      if (rc_LoggingData.e_OverwriteMode == C_CamProLoggingData::eWARN_BEFORE_OVERWRITE)
      {
         if (c_FileInfo.exists() == true)
         {
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eWARNING);
            c_Message.SetHeading(C_GtGetText::h_GetText("Log file overwrite"));
            c_Message.SetDescription(static_cast<QString>(C_GtGetText::h_GetText("The log file %1 already exists. "
                                                                                 "Do you want to overwrite the file?")).
                                     arg(c_FileInfo.absoluteFilePath()));
            c_Message.SetCancelButtonText(C_GtGetText::h_GetText("Cancel"));
            c_Message.SetOkButtonText(C_GtGetText::h_GetText("Overwrite"));
            q_Continue = (c_Message.Execute() == C_OgeWiCustomMessage::eOK);
         }
      }
   }

   if (q_Continue == true)
   {
      // start logging
      if (rc_LoggingData.e_FileFormat == C_CamProLoggingData::eBLF)
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
