//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for save-as dialog (implementation)

   Widget for save-as dialog. Most functionality copied/moved from
   C_PopSaveAsWidget.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.07.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
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

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_errors;

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

   \param[in,out] orc_Parent Reference to parent

   \created     26.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
C_PopSaveAsDialogWidget::C_PopSaveAsDialogWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_PopSaveAsDialogWidget),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   // init names
   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   //Default input
   m_InitDefaultProjectName();

   // no strange cursor
   QApplication::restoreOverrideCursor();

   connect(this->mpc_Ui->pc_PushButtonBrowse, &QPushButton::clicked, this, &C_PopSaveAsDialogWidget::m_OnBrowse);
   connect(this->mpc_Ui->pc_PushButtonSave, &QPushButton::clicked, this, &C_PopSaveAsDialogWidget::m_OnSave);
   connect(this->mpc_Ui->pc_PushButtonCancel, &C_OgePubCancel::clicked, this, &C_PopSaveAsDialogWidget::m_OnCancel);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     26.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
C_PopSaveAsDialogWidget::~C_PopSaveAsDialogWidget(void)
{
   delete this->mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     26.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_PopSaveAsDialogWidget::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("PROJECT OPERATION"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Save Project as"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Location"));
   this->mpc_Ui->pc_LabelName->setText(C_GtGetText::h_GetText("Project Name"));
   this->mpc_Ui->pc_LabelPath->setText(C_GtGetText::h_GetText("Path"));
   this->mpc_Ui->pc_LineEditName->setPlaceholderText(C_GtGetText::h_GetText(""));
   this->mpc_Ui->pc_LineEditPath->setPlaceholderText(C_GtGetText::h_GetText(""));
   this->mpc_Ui->pc_PushButtonBrowse->setText(C_GtGetText::h_GetText("..."));
   this->mpc_Ui->pc_PushButtonSave->setText(C_GtGetText::h_GetText("Save"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle save user settings operation

   \created     25.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_PopSaveAsDialogWidget::SaveUserSettings() const
{
   C_UsHandler::h_GetInstance()->SetCurrentSaveAsPath(this->mpc_Ui->pc_LineEditPath->GetCompletePath());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information

   \created     26.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Handle default project name

   \created     01.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PopSaveAsDialogWidget::m_InitDefaultProjectName(void) const
{
   const QString c_ProjectPath = C_PuiProject::h_GetInstance()->GetPath();
   QString c_Proposal;

   this->mpc_Ui->pc_LineEditPath->SetPath(m_GetValidPath(C_UsHandler::h_GetInstance()->GetCurrentSaveAsPath()));

   if (c_ProjectPath.compare("") == 0)
   {
      c_Proposal = "NewProject";
   }
   else
   {
      QFileInfo c_Project(c_ProjectPath);
      //Translation: 1: Project name
      c_Proposal = QString("Copy_of_%1").arg(c_Project.baseName());
   }
   this->mpc_Ui->pc_LineEditName->setText(c_Proposal);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save project to file

   \param[in] orc_File File

   \return
   see C_PuiProject::h_GetInstance()->Save(...) for return values

   \created     26.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
sint32 C_PopSaveAsDialogWidget::m_SaveToFile(const QString & orc_File) const
{
   sint32 s32_Return;

   // remember old project path
   const QString c_RememberPath = C_PuiProject::h_GetInstance()->GetPath();

   // try to save and return errors else
   C_PuiProject::h_GetInstance()->SetPath(orc_File);
   s32_Return = C_PuiProject::h_GetInstance()->Save(true);
   C_PopErrorHandling::mh_ProjectSaveErr(s32_Return, this->parentWidget());

   if (s32_Return == C_NO_ERR)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Replace empty path if necessary

   \param[in] orc_Path Some path

   \return
   Non empty path

   \created     26.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
QString C_PopSaveAsDialogWidget::m_GetValidPath(const QString & orc_Path) const
{
   QString c_Retval = orc_Path;

   if (c_Retval.compare("") == 0)
   {
      c_Retval = C_Uti::h_GetExePath();
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle browse event

   \created     26.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_PopSaveAsDialogWidget::m_OnBrowse(void)
{
   QString c_Folder = m_GetValidPath(this->mpc_Ui->pc_LineEditPath->GetCompletePath());

   const QString c_Path =
      QFileDialog::getExistingDirectory(this, C_GtGetText::h_GetText("Choose location to save the project"), c_Folder,
                                        QFileDialog::ShowDirsOnly);

   if (c_Path.compare("") != 0)
   {
      this->mpc_Ui->pc_LineEditPath->SetPath(c_Path);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Ok button click

   \created     26.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_PopSaveAsDialogWidget::m_OnSave(void)
{
   QApplication::setOverrideCursor(Qt::WaitCursor);

   const QString c_BasePath = this->mpc_Ui->pc_LineEditPath->GetCompletePath();
   const QString c_Name = this->mpc_Ui->pc_LineEditName->text();

   if ((c_BasePath.compare("") != 0) && (c_Name.compare("") != 0))
   {
      if (C_OSCUtils::h_CheckValidCName(c_Name.toStdString().c_str(), std::numeric_limits<uint16>::max()))
      {
         const QString c_Path = c_BasePath + '/' + c_Name;
         const QString c_FilePathAndName = c_Path + '/' + c_Name + ".syde";
         QDir c_Dir(c_Path);

         if (c_Dir.exists() == false)
         {
            c_Dir.mkdir(c_Path);
            if (m_SaveToFile(c_FilePathAndName) == C_NO_ERR)
            {
               // accept dialog if successfully saved
               this->mrc_ParentDialog.accept();
            }
            QApplication::restoreOverrideCursor();
         }
         else
         {
            QApplication::restoreOverrideCursor();
            C_OgeWiCustomMessage c_Box(this, C_OgeWiCustomMessage::E_Type::eERROR);
            c_Box.SetHeading(C_GtGetText::h_GetText("Project save"));
            c_Box.SetDescription(C_GtGetText::h_GetText(
                                    "A project with this name already exists. Please choose another name."));
            c_Box.Execute();
         }
      }
      else
      {
         QApplication::restoreOverrideCursor();
         C_OgeWiCustomMessage c_Box(this, C_OgeWiCustomMessage::E_Type::eERROR);
         c_Box.SetHeading(C_GtGetText::h_GetText("Project save"));
         c_Box.SetDescription(C_GtGetText::h_GetText(
                                 "Project name invalid. Only alphanumeric characters + \"_\" are allowed."));
         c_Box.Execute();
      }
   }
   else
   {
      QApplication::restoreOverrideCursor();
      const QString c_LName = this->mpc_Ui->pc_LabelName->text();
      const QString c_Path = this->mpc_Ui->pc_LabelPath->text();
      C_OgeWiCustomMessage c_Box(this, C_OgeWiCustomMessage::E_Type::eERROR);
      c_Box.SetHeading(C_GtGetText::h_GetText("Save project as"));
      //Translation: 1: Project name 2: Project path
      c_Box.SetDescription(QString(C_GtGetText::h_GetText("Please insert a valid string for both %1 and %2.")).
                           arg(c_LName.toLower(), c_Path.toLower()));
      c_Box.Execute();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Cancel button click


   \created     26.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_PopSaveAsDialogWidget::m_OnCancel(void)
{
   this->mrc_ParentDialog.reject();
}
