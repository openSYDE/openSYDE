//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for save-as dialog (implementation)

   Widget for save-as dialog. Most functionality copied/moved from
   C_PopSaveAsWidget.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>
#include <QFileDialog>

#include "stwtypes.h"
#include "C_OSCUtils.h"
#include "C_GtGetText.h"
#include "C_PuiProject.h"
#include "C_PopErrorHandling.h"
#include "C_OgeWiCustomMessage.h"
#include "C_Uti.h"
#include "C_HeHandler.h"
#include "stwerrors.h"

#include "C_PopSaveAsDialogWidget.h"
#include "ui_C_PopSaveAsDialogWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_types::sintn C_PopSaveAsDialogWidget::mhsn_VERSION_INDEX_V2 = 1;
const stw_types::sintn C_PopSaveAsDialogWidget::mhsn_VERSION_INDEX_V3 = 0;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_PopSaveAsDialogWidget::C_PopSaveAsDialogWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_PopSaveAsDialogWidget),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   // init names
   InitStaticNames();

   this->mpc_Ui->pc_LineEditPath->SetDragAndDropActiveForFolder(true);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   //Default input
   m_InitDefaultProjectName();

   // no strange cursor
   QApplication::restoreOverrideCursor();

   // Remove "..." string
   this->mpc_Ui->pc_PushButtonBrowse->setText("");

   connect(this->mpc_Ui->pc_PushButtonBrowse, &QPushButton::clicked, this, &C_PopSaveAsDialogWidget::m_OnBrowse);
   connect(this->mpc_Ui->pc_PushButtonSave, &QPushButton::clicked, this, &C_PopSaveAsDialogWidget::m_OnSave);
   connect(this->mpc_Ui->pc_PushButtonCancel, &C_OgePubCancel::clicked, this, &C_PopSaveAsDialogWidget::m_OnCancel);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PopSaveAsDialogWidget::~C_PopSaveAsDialogWidget(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopSaveAsDialogWidget::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Project"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Save As"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Location"));
   this->mpc_Ui->pc_LabelName->setText(C_GtGetText::h_GetText("Project Name"));
   this->mpc_Ui->pc_LabelPath->setText(C_GtGetText::h_GetText("Path"));
   this->mpc_Ui->pc_LabelVersion->setText(C_GtGetText::h_GetText("File Format"));
   this->mpc_Ui->pc_ComboBoxVersion->addItem(C_GtGetText::h_GetText("V3 (default)"));
   this->mpc_Ui->pc_ComboBoxVersion->addItem(C_GtGetText::h_GetText("V2"));
   this->mpc_Ui->pc_LineEditName->setPlaceholderText(C_GtGetText::h_GetText(""));
   this->mpc_Ui->pc_LineEditPath->setPlaceholderText(C_GtGetText::h_GetText(""));
   this->mpc_Ui->pc_PushButtonSave->setText(C_GtGetText::h_GetText("Save"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   //Tool tips
   this->mpc_Ui->pc_LabelName->SetToolTipInformation(C_GtGetText::h_GetText("Project Name"),
                                                     C_GtGetText::h_GetText(
                                                        "Project name to choose. This will also be used as the default folder name your project is saved in."));
   this->mpc_Ui->pc_LabelPath->SetToolTipInformation(C_GtGetText::h_GetText("Path"),
                                                     C_GtGetText::h_GetText(
                                                        "Path to create the new folder for the new project in"));
   this->mpc_Ui->pc_LabelVersion->SetToolTipInformation(C_GtGetText::h_GetText("File Format"),
                                                        C_GtGetText::h_GetText(
                                                           "File format to use when saving this project.\n"
                                                           "V3 (default): Project split into multiple files to improve support for multi user edit of any system definition properties\n"
                                                           "V2: Compatibility mode for previous versions of provided tools (not recommended as this does not save all supported project properties)"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle save user settings operation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopSaveAsDialogWidget::SaveUserSettings() const
{
   C_UsHandler::h_GetInstance()->SetCurrentSaveAsPath(this->mpc_Ui->pc_LineEditPath->GetPath());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopSaveAsDialogWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
         // behave like ok click
         this->m_OnSave();
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
/*! \brief   Handle default project name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopSaveAsDialogWidget::m_InitDefaultProjectName(void) const
{
   const QString c_ProjectPath = C_PuiProject::h_GetInstance()->GetPath();
   QString c_Proposal;
   QString c_AbsolutePath = static_cast<QFileInfo>(m_GetValidPath(c_ProjectPath)).absoluteDir().absolutePath();

   c_AbsolutePath.remove(c_AbsolutePath.lastIndexOf("/"), c_AbsolutePath.length());

   this->mpc_Ui->pc_LineEditPath->SetPath(c_AbsolutePath);

   if (c_ProjectPath.compare("") == 0)
   {
      c_Proposal = "NewProject";
   }
   else
   {
      c_Proposal = static_cast<QString>("Copy_of_%1").arg(C_PuiProject::h_GetInstance()->GetName());
   }
   this->mpc_Ui->pc_LineEditName->setText(c_Proposal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save project to file

   \param[in] orc_File                     File
   \param[in] oq_UseDeprecatedFileFormatV2 Flag to enable saving using the deprecated V2 file format

   \return
   see C_PuiProject::h_GetInstance()->Save(...) for return values
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PopSaveAsDialogWidget::m_SaveToFile(const QString & orc_File, const bool oq_UseDeprecatedFileFormatV2) const
{
   sint32 s32_Return;

   // remember old project path
   const QString c_RememberPath = C_PuiProject::h_GetInstance()->GetPath();

   // try to save and return errors else
   C_PuiProject::h_GetInstance()->SetPath(orc_File);
   s32_Return = C_PuiProject::h_GetInstance()->Save(true, oq_UseDeprecatedFileFormatV2);
   C_PopErrorHandling::h_ProjectSaveErr(s32_Return, this->parentWidget());

   //Only use new path if not saved in deprecated format
   if ((s32_Return == C_NO_ERR) && (oq_UseDeprecatedFileFormatV2 == false))
   {
      // use real path for active project and recent projects
      const QString c_Path = C_PuiProject::h_GetInstance()->GetPath();
      C_UsHandler::h_GetInstance()->AddToRecentProjects(c_Path);
      C_UsHandler::h_GetInstance()->ChangeActiveProjectName(c_Path);
   }
   else
   {
      // reset to previous project path
      C_PuiProject::h_GetInstance()->SetPath(c_RememberPath);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Replace empty path if necessary

   \param[in] orc_Path Some path

   \return
   Non empty path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PopSaveAsDialogWidget::m_GetValidPath(const QString & orc_Path) const
{
   QString c_Retval = orc_Path;

   if (c_Retval.compare("") == 0)
   {
      c_Retval = C_Uti::h_GetExePath();
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle browse event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopSaveAsDialogWidget::m_OnBrowse(void)
{
   const QString c_Folder = m_GetValidPath(this->mpc_Ui->pc_LineEditPath->GetPath());

   const QString c_Path =
      QFileDialog::getExistingDirectory(this, C_GtGetText::h_GetText("Select Project Location"), c_Folder,
                                        QFileDialog::ShowDirsOnly);

   if (c_Path.compare("") != 0)
   {
      this->mpc_Ui->pc_LineEditPath->SetPath(c_Path);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopSaveAsDialogWidget::m_OnSave(void)
{
   QApplication::setOverrideCursor(Qt::WaitCursor);

   const QString c_BasePath = this->mpc_Ui->pc_LineEditPath->GetPath();
   const QString c_Name = this->mpc_Ui->pc_LineEditName->text();

   const bool q_ValidName = C_OSCUtils::h_CheckValidFileName(c_Name.toStdString().c_str());
   const bool q_ValidPath = C_OSCUtils::h_CheckValidFilePath(c_BasePath.toStdString().c_str());

   if ((q_ValidName == true) && (q_ValidPath == true))
   {
      const QString c_Path = c_BasePath + '/' + c_Name;
      const QString c_FilePathAndName = c_Path + '/' + c_Name + ".syde";
      const QDir c_Dir(c_Path);

      if (c_Dir.exists() == false)
      {
         c_Dir.mkdir(c_Path);
         if (this->mpc_Ui->pc_ComboBoxVersion->currentIndex() == C_PopSaveAsDialogWidget::mhsn_VERSION_INDEX_V2)
         {
            C_OgeWiCustomMessage c_Box(this, C_OgeWiCustomMessage::eINFORMATION);
            c_Box.SetHeading(C_GtGetText::h_GetText("Project save as \"V2\""));
            c_Box.SetDescription(C_GtGetText::h_GetText("Project is exported in file format \"V2\" "
                                                        "as a copy of the current project.\n"
                                                        "You are still working on current project."));
            c_Box.SetOKButtonText(C_GtGetText::h_GetText("Continue"));
            c_Box.SetNOButtonText(C_GtGetText::h_GetText("Cancel"));
            c_Box.SetCustomMinHeight(180, 180);
            if (c_Box.Execute() == C_OgeWiCustomMessage::eOK)
            {
               if (m_SaveToFile(c_FilePathAndName, true) == C_NO_ERR)
               {
                  // accept dialog if successfully saved
                  this->mrc_ParentDialog.accept();
               }
            }
            else
            {
               c_Dir.rmdir(c_Path);
            }
         }
         else
         {
            if (m_SaveToFile(c_FilePathAndName, false) == C_NO_ERR)
            {
               // accept dialog if successfully saved
               this->mrc_ParentDialog.accept();
            }
         }
         QApplication::restoreOverrideCursor();
      }
      else
      {
         C_OgeWiCustomMessage c_Box(this, C_OgeWiCustomMessage::E_Type::eERROR);
         QApplication::restoreOverrideCursor();
         c_Box.SetHeading(C_GtGetText::h_GetText("Project save"));
         c_Box.SetDescription(C_GtGetText::h_GetText("A project with this name already exists. Choose another name."));
         c_Box.SetCustomMinHeight(180, 180);
         c_Box.Execute();
      }
   }
   else
   {
      C_OgeWiCustomMessage c_Box(this, C_OgeWiCustomMessage::E_Type::eERROR);
      QApplication::restoreOverrideCursor();
      QString c_Details = C_GtGetText::h_GetText("Invalid content: \n");
      if (q_ValidName == false)
      {
         c_Details += (c_Name.isEmpty()) ?  C_GtGetText::h_GetText("Empty project name") : c_Name;
         c_Details += "\n";
      }

      if (q_ValidPath == false)
      {
         c_Details += (c_BasePath.isEmpty()) ?  C_GtGetText::h_GetText("Empty project path") : c_BasePath;
      }

      c_Box.SetHeading(C_GtGetText::h_GetText("Save project as"));
      c_Box.SetDescription(C_GtGetText::h_GetText("Name or/and path is empty or contains invalid characters. "
                                                  "Please choose valid name and path."));
      c_Box.SetDetails(c_Details);
      c_Box.SetCustomMinHeight(230, 270);
      c_Box.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopSaveAsDialogWidget::m_OnCancel(void)
{
   this->mrc_ParentDialog.reject();
}
