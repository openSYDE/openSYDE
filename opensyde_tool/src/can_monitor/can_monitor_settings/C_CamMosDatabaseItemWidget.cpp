//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for Database list items (implementation)

   Widget for Database list items including a checkbox, database name, browse button and remove button.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QFileDialog>

#include "C_CamMosDatabaseItemWidget.hpp"
#include "ui_C_CamMosDatabaseItemWidget.h"

#include "C_OgePopUpDialog.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_Uti.hpp"
#include "constants.hpp"
#include "C_CamMosDatabaseBusSelectionPopup.hpp"
#include "C_UsHandler.hpp"
#include "C_CamProHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_CamUti.hpp"
#include "C_CamUtiGeneric.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      orc_Database  Database data
   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosDatabaseItemWidget::C_CamMosDatabaseItemWidget(const C_CamProDatabaseData & orc_Database,
                                                       QWidget * const opc_Parent) :
   stw::opensyde_gui_elements::C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_CamMosDatabaseItemWidget),
   me_State(eLOADING),
   mq_AlreadyAskedUserReload(true),
   mq_AlreadyAskedUserDelete(false),
   mq_ButtonPressed(false),
   mq_SuppressChangeSignal(false)
{
   this->mpc_Ui->setupUi(this);

   // initialize database
   this->mc_Database = orc_Database;
   this->mc_FileTimeStamp = static_cast<QFileInfo>(C_CamUti::h_GetAbsPathFromProj(orc_Database.c_Name)).lastModified();

   // reset buses
   this->mc_Busses.clear();

   // check box
   this->mpc_Ui->pc_CheckBox->setChecked(mc_Database.q_Enabled);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_CheckBox, "NoText", true);

   // icon is set from outside

   // label
   this->mpc_Ui->pc_LabDatabase->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabDatabase->SetForegroundColor(0);
   this->mpc_Ui->pc_LabDatabase->SetFontPixel(13);

   this->m_SetMinimizedPath();
   this->UpdateTooltip();

   // buttons
   this->mpc_Ui->pc_PbSelectBus->SetSvg("://images/IconEdit.svg", "://images/IconNothing.svg",
                                        "://images/IconEditHovered.svg");
   this->mpc_Ui->pc_PbBrowse->SetSvg("://images/IconBrowse.svg", "://images/IconNothing.svg",
                                     "://images/IconBrowseHovered.svg");
   this->mpc_Ui->pc_PbRemove->SetSvg("://images/IconClearGray.svg", "://images/IconNothing.svg",
                                     "://images/IconClearGrayHovered.svg");

   this->mpc_Ui->pc_PbSelectBus->SetToolTipInformation(C_GtGetText::h_GetText("Select Bus."),
                                                       C_GtGetText::h_GetText("Select bus for this database."));
   this->mpc_Ui->pc_PbBrowse->SetToolTipInformation(C_GtGetText::h_GetText("Browse"),
                                                    C_GtGetText::h_GetText("Choose database file "
                                                                           "(*.syde_sysdef or *.dbc)."));
   this->mpc_Ui->pc_PbRemove->SetToolTipInformation(C_GtGetText::h_GetText("Delete"),
                                                    C_GtGetText::h_GetText("Delete this database."));
   this->m_EnableCheckBoxTooltip(true); // set tooltip of pc_CheckBox

   // hide buttons on start
   this->mpc_Ui->pc_PbSelectBus->setVisible(false);
   this->mpc_Ui->pc_PbBrowse->setVisible(false);
   this->mpc_Ui->pc_PbRemove->setVisible(false);

   // connects
   connect(this->mpc_Ui->pc_PbSelectBus, &C_OgePubSvgIconOnly::clicked, this,
           &C_CamMosDatabaseItemWidget::m_OnSelectBus);
   connect(this->mpc_Ui->pc_PbBrowse, &C_OgePubSvgIconOnly::clicked, this, &C_CamMosDatabaseItemWidget::m_OnBrowse);
   connect(this->mpc_Ui->pc_PbRemove, &C_OgePubSvgIconOnly::clicked, this, &C_CamMosDatabaseItemWidget::m_OnRemove);
   connect(this->mpc_Ui->pc_CheckBox, &QCheckBox::toggled, this, &C_CamMosDatabaseItemWidget::m_OnChxToggle);

   connect(this->mpc_Ui->pc_PbSelectBus, &C_OgePubSvgIconOnly::pressed, this,
           &C_CamMosDatabaseItemWidget::m_ButtonPressed);
   connect(this->mpc_Ui->pc_PbBrowse, &C_OgePubSvgIconOnly::pressed, this,
           &C_CamMosDatabaseItemWidget::m_ButtonPressed);
   connect(this->mpc_Ui->pc_PbRemove, &C_OgePubSvgIconOnly::pressed, this,
           &C_CamMosDatabaseItemWidget::m_ButtonPressed);
   connect(this->mpc_Ui->pc_PbSelectBus, &C_OgePubSvgIconOnly::released, this,
           &C_CamMosDatabaseItemWidget::m_ButtonReleased);
   connect(this->mpc_Ui->pc_PbBrowse, &C_OgePubSvgIconOnly::released, this,
           &C_CamMosDatabaseItemWidget::m_ButtonReleased);
   connect(this->mpc_Ui->pc_PbRemove, &C_OgePubSvgIconOnly::released, this,
           &C_CamMosDatabaseItemWidget::m_ButtonReleased);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosDatabaseItemWidget::~C_CamMosDatabaseItemWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set icon and tool tip depending on state.

   \param[in]  oe_State    State
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseItemWidget::SetState(const C_CamMosDatabaseItemWidget::E_LoadingState oe_State)
{
   QString c_Icon = "";
   QString c_ToolTip = "";
   bool q_EnableButtons = true;
   bool q_EnableBusSelectButton = false;
   bool q_Error = false;

   C_NagToolTip::E_Type e_Type = C_NagToolTip::E_Type::eDEFAULT;
   this->me_State = oe_State;

   switch (oe_State)
   {
   case eOK:
      c_Icon = "://images/IconFile.svg";
      c_ToolTip = C_GtGetText::h_GetText("Database successfully loaded and ready for use.");
      q_EnableBusSelectButton = true;
      break;
   case eLOADING:
      c_Icon = "://images/IconFileLoading.svg";
      c_ToolTip = C_GtGetText::h_GetText("Loading database ...");
      q_EnableButtons = false;
      break;
   case eQUEUE:
      c_Icon = "://images/IconFileQueue.svg";
      c_ToolTip = C_GtGetText::h_GetText("Database is in queue.");
      break;
   case eNOT_FOUND:
      c_ToolTip = C_GtGetText::h_GetText("File not found. This database will not be used.");
      q_Error = true;
      break;
   case eLOAD_ERROR:
      c_ToolTip = C_GtGetText::h_GetText("Error loading file. This database will not be used.");
      q_Error = true;
      break;
   case eWRONG_FORMAT:
      c_ToolTip = C_GtGetText::h_GetText(
         "File format is not supported. Only files of format *.dbc and *.syde_sysdef are supported.\n"
         "This database will not be used.");
      q_Error = true;
      break;
   default:
      c_ToolTip = C_GtGetText::h_GetText("Unknown Error. This database will not be used.");
      q_Error = true;
      break;
   }

   if (q_Error == true)
   {
      c_Icon = "://images/IconFileError.svg";
      e_Type = C_NagToolTip::E_Type::eERROR;
   }

   // adapt icon and its tooltip
   this->mpc_Ui->pc_LabIcon->SetSvg(c_Icon);
   this->mpc_Ui->pc_LabIcon->SetToolTipInformation("Database Status", c_ToolTip, e_Type);

   // disable remove and browse in loading state
   this->mpc_Ui->pc_PbBrowse->setEnabled(q_EnableButtons);
   this->mpc_Ui->pc_PbRemove->setEnabled(q_EnableButtons);

   // enable select bus button only on success
   this->mpc_Ui->pc_PbSelectBus->setEnabled(q_EnableBusSelectButton);

   // disable label and checkbox in error case and enable else
   this->mpc_Ui->pc_LabDatabase->setEnabled(!q_Error);
   this->mpc_Ui->pc_CheckBox->setEnabled(!q_Error);
   this->m_EnableCheckBoxTooltip(!q_Error);

   // in error case only set unchecked
   if (q_Error == true)
   {
      this->mq_SuppressChangeSignal = true;
      this->mpc_Ui->pc_CheckBox->setChecked(false);
      this->mq_SuppressChangeSignal = false;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set bus index of database data.

   \param[in]  ou32_BusIndex  bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseItemWidget::SetBusIndex(const uint32_t ou32_BusIndex)
{
   this->mc_Database.s32_BusIndex = static_cast<int32_t>(ou32_BusIndex);
   this->UpdateTooltip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remember all buses of openSYDE system definition for future selection.

   \param[in]  orc_Busses  buses
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseItemWidget::SetBusses(const std::vector<stw::opensyde_core::C_OscSystemBus> & orc_Busses)
{
   this->mc_Busses = orc_Busses;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update tool tip: total path and bus index.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseItemWidget::UpdateTooltip(void) const
{
   const QString c_AbsolutePath = C_CamUti::h_GetAbsPathFromProj(this->mc_Database.c_Name);
   QString c_ToolTipContent = c_AbsolutePath;

   if (this->mc_Database.c_Name != c_AbsolutePath)
   {
      c_ToolTipContent = C_GtGetText::h_GetText("File path: ") + this->mc_Database.c_Name + "\n";
      c_ToolTipContent += C_GtGetText::h_GetText("File path absolute: ") + c_AbsolutePath;
   }

   if ((this->mc_Database.s32_BusIndex > -1) &&
       (this->mc_Database.s32_BusIndex < static_cast<int32_t>(this->mc_Busses.size())) &&
       (static_cast<QFileInfo>(c_AbsolutePath).suffix().compare("syde_sysdef", Qt::CaseInsensitive) == 0))
   {
      c_ToolTipContent += "\n\n";
      c_ToolTipContent +=
         static_cast<QString>(C_GtGetText::h_GetText("Bus: %1")).arg(
            this->mc_Busses[this->mc_Database.s32_BusIndex].c_Name.c_str());
   }

   this->mpc_Ui->pc_LabDatabase->SetToolTipInformation(static_cast<QFileInfo>(this->mc_Database.c_Name).fileName(),
                                                       c_ToolTipContent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update database name to react if relativeness changed.

   \param[in]  orc_NewDatabaseName  new database path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseItemWidget::UpdateDatabasePathRelativeness(const QString & orc_NewDatabaseName)
{
   this->mc_Database.c_Name = orc_NewDatabaseName;

   // update GUI
   this->m_SetMinimizedPath();
   this->UpdateTooltip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for cyclic checking of file changes.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseItemWidget::CheckFile(void)
{
   bool q_Remove = false;

   // only reload in the following cases, i.e. wait till file was loaded
   if ((this->me_State == C_CamMosDatabaseItemWidget::eOK) ||
       (this->me_State == C_CamMosDatabaseItemWidget::eNOT_FOUND) ||
       (this->me_State == C_CamMosDatabaseItemWidget::eLOAD_ERROR))
   {
      const QFileInfo c_File(C_CamUti::h_GetAbsPathFromProj(this->mc_Database.c_Name));
      C_OgeWiCustomMessage c_Message(this);
      static uint8_t hu8_FileOpenFailCounter = 0;

      // check file existence
      if ((c_File.exists() == true) && (c_File.isFile() == true))
      {
         const QDateTime c_Timestamp = c_File.lastModified();
         //reset fail counter
         hu8_FileOpenFailCounter = 0;

         // check if new timestamp
         if (this->mc_FileTimeStamp == c_Timestamp)
         {
            if (this->mq_AlreadyAskedUserReload == false)
            {
               // ask user to reload database
               c_Message.SetType(C_OgeWiCustomMessage::eQUESTION);
               c_Message.SetHeading(C_GtGetText::h_GetText("File Changed"));
               c_Message.SetDescription(
                  static_cast<QString>(C_GtGetText::h_GetText(
                                          "Database %1 changed on disk. Do you want to reload this database?")).
                  arg(c_File.absoluteFilePath()));
               c_Message.SetOkButtonText(C_GtGetText::h_GetText("Reload"));
               c_Message.SetNoButtonText(C_GtGetText::h_GetText("Don't reload!"));

               if (c_Message.Execute() == C_OgeWiCustomMessage::eOK)
               {
                  // reload database
                  Q_EMIT (this->SigUpdateDatabasePath(this, this->mc_Database, true));
               }

               this->mq_AlreadyAskedUserReload = true;
            }
         }
         else
         {
            // reset
            this->mc_FileTimeStamp = c_Timestamp;
            this->mq_AlreadyAskedUserReload = false;
         }

         // reset delete flag
         this->mq_AlreadyAskedUserDelete = false;
      }
      else
      {
         //File not found. Inrease fail counter
         hu8_FileOpenFailCounter++;

         //From time to time there are false positive messages (e.g.: when openSYDE GUI is saving the project and
         //at the same time openSYDE CAN Monitor is checking if database has been changed)
         //To avoid this messages simple approach is implemented: handle "file not found" case only after second occur
         // (= forgive one fail)
         if (hu8_FileOpenFailCounter > 1)
         {
            //reset for next use
            hu8_FileOpenFailCounter = 0;

            if (this->mq_AlreadyAskedUserDelete == false)
            {
               QString c_Details;
               QString c_Description = static_cast<QString>(C_GtGetText::h_GetText("Could not find file %1. "
                                                                                   "Do you want to delete this database?"))
                                       .arg(
                  c_File.absoluteFilePath());
               C_CamMosDatabaseItemWidget::h_AppendMessageWarningIfNecessary(
                  c_File.absoluteFilePath(), c_Description, c_Details);
               // ask user to delete database
               c_Message.SetType(C_OgeWiCustomMessage::eERROR);
               c_Message.SetHeading(C_GtGetText::h_GetText("File not found"));
               c_Message.SetDescription(c_Description);
               c_Message.SetDetails(c_Details);
               c_Message.SetOkButtonText(C_GtGetText::h_GetText("Delete"));
               c_Message.SetCancelButtonText(C_GtGetText::h_GetText("Keep"));
               if (c_Message.Execute() == C_OgeWiCustomMessage::eOK)
               {
                  q_Remove = true;
               }
               else
               {
                  Q_EMIT (this->SigNotifyMissingDataBase(this->mc_Database.c_Name));
               }

               // reset flag
               this->mq_AlreadyAskedUserDelete = true;
            }
         }

         // reset reload flag
         this->mq_AlreadyAskedUserReload = false;
      }
   }

   // remove (last action!)
   if (q_Remove == true)
   {
      Q_EMIT (this->SigRemoveDatabase(this, false));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get associated database data.

   \return
   database data
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamProDatabaseData C_CamMosDatabaseItemWidget::GetDatabaseData(void) const
{
   return this->mc_Database;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Browse for database path and check if already used

   \param[in]  opc_Parent  pointer to parent (to integrate browse window correctly)

   \return
   new database path (empty if already existing or user canceled)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamMosDatabaseItemWidget::h_BrowseForDatabasePath(QWidget * const opc_Parent)
{
   const C_CamMosDatabaseItemWidget * const pc_DatabaseItemWidget =
      dynamic_cast<C_CamMosDatabaseItemWidget *>(opc_Parent);
   const QString c_Filter = static_cast<QString>(C_GtGetText::h_GetText("CAN Database File ")) +
                            "(*.syde_sysdef | *.dbc | *.syde)";
   QString c_Folder = "";
   QString c_Return = "";
   QFileDialog c_Dialog(opc_Parent, C_GtGetText::h_GetText("Select Database"), c_Folder, c_Filter);

   // favorite location: actual database path if browse was clicked
   if (pc_DatabaseItemWidget != NULL)
   {
      c_Folder =
         static_cast<QFileInfo>(C_CamUti::h_GetAbsPathFromProj(pc_DatabaseItemWidget->GetDatabaseData().c_Name)).dir().
         absolutePath();
   }

   // second favorite location: last known database path
   if (c_Folder == "")
   {
      c_Folder = C_UsHandler::h_GetInstance()->GetLastKnownDatabasePath();

      // third favorite location: project location
      if (c_Folder == "")
      {
         c_Folder = C_CamProHandler::h_GetInstance()->GetCurrentProjDir();

         // fall back location: path of exe
         if (c_Folder == "")
         {
            c_Folder = C_Uti::h_GetExePath();
         }
      }
   }

   c_Dialog.setDirectory(c_Folder);
   c_Dialog.setDefaultSuffix(".syde_sysdef");
   c_Dialog.setFileMode(QFileDialog::ExistingFile);

   if (c_Dialog.exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      c_Return = c_Dialog.selectedFiles().at(0); // always absolute so no problems
      // "" is returned if user canceled
      if (c_Return != "")
      {
         const QFileInfo c_FileInfo = static_cast<QFileInfo>(c_Return);
         // store new user settings
         C_UsHandler::h_GetInstance()->SetLastKnownDatabasePath(c_FileInfo.absoluteDir().absolutePath());

         // if necessary adapt .syde path to .syde_sysdef
         c_Return = C_CamMosDatabaseItemWidget::h_AdaptPathToSystemDefinitionIfNecessary(c_FileInfo);

         // check if database is already used
         if (C_CamMosDatabaseItemWidget::h_IsDatabaseAlreadyUsed(c_Return, opc_Parent) == true)
         {
            // return empty path
            c_Return = "";
         }
      }
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if database is already used and inform user if so

   Make sure that input QFileInfo is valid (i.e. created with absolute and nonempty path).

   \param[in]  orc_SydeFile   absolute path to .syde file

   \return
   system definition path (always absolute)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamMosDatabaseItemWidget::h_AdaptPathToSystemDefinitionIfNecessary(const QFileInfo & orc_SydeFile)
{
   QString c_Return = orc_SydeFile.absoluteFilePath();

   if (orc_SydeFile.suffix().compare("syde", Qt::CaseInsensitive) == 0)
   {
      // first try file version V3, i.e. system definition file is in sub directory
      QString c_SysdefFile =
         orc_SydeFile.absolutePath() + "/system_definition/" + orc_SydeFile.completeBaseName() + ".syde_sysdef";

      if (static_cast<QFileInfo>(c_SysdefFile).exists() == true)
      {
         c_Return = c_SysdefFile;
      }
      else
      {
         // now try file version V2, i.e. system definition file is beneath .syde file
         c_SysdefFile = orc_SydeFile.absolutePath() + "/" + orc_SydeFile.completeBaseName() + ".syde_sysdef";

         if (static_cast<QFileInfo>(c_SysdefFile).exists() == true)
         {
            c_Return = c_SysdefFile;
         }
      }
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if database is already used and inform user if so

   \param[in]  orc_AbsolutePath  absolute path to new database
   \param[in]  opc_Parent        pointer to parent (to integrate browse window correctly)

   \retval true: database already used
   \retval false: database not present
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMosDatabaseItemWidget::h_IsDatabaseAlreadyUsed(const QString & orc_AbsolutePath, QWidget * const opc_Parent)
{
   bool q_Return = false;

   // check if database already exists
   for (uint32_t u32_Pos = 0; u32_Pos < C_CamProHandler::h_GetInstance()->GetDatabases().size(); u32_Pos++)
   {
      if (C_CamUti::h_GetAbsPathFromProj(C_CamProHandler::h_GetInstance()->GetDatabases()[u32_Pos].c_Name) ==
          orc_AbsolutePath) // compare absolute paths
      {
         // inform user
         C_OgeWiCustomMessage c_Message(opc_Parent);
         c_Message.SetHeading(C_GtGetText::h_GetText("Database Loading"));
         c_Message.SetDescription(
            static_cast<QString>(C_GtGetText::h_GetText("The database %1 is already used and not added again.")).
            arg(orc_AbsolutePath));
         c_Message.Execute();

         // return true
         q_Return = true;
         break;
      }
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append message warning to database delete request if necessary

   \param[in]      orc_File         File to delete
   \param[in,out]  orc_Description  Current description
   \param[in,out]  orc_Details      Current details
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseItemWidget::h_AppendMessageWarningIfNecessary(const QString & orc_File, QString & orc_Description,
                                                                   QString & orc_Details)
{
   QStringList c_Messages;

   //Handle messages section
   C_CamProHandler::h_GetInstance()->GetAllMessagesFromDatabase(orc_File, &c_Messages, NULL);
   //Only add special handling if there are any affected messages
   if (c_Messages.isEmpty() == false)
   {
      //Append description
      orc_Description.append(C_GtGetText::h_GetText(
                                " All messages generator messages from this database will be deleted."));
      //Add details
      orc_Details.append(C_GtGetText::h_GetText(
                            "Following messages will be deleted:\n"));
      //Add all messages
      for (QStringList::const_iterator c_It = c_Messages.begin(); c_It != c_Messages.end(); ++c_It)
      {
         orc_Details.append(*c_It);
         orc_Details.append("\n");
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for select bus button.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseItemWidget::m_OnSelectBus()
{
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   const C_CamMosDatabaseBusSelectionPopup * const pc_Dialog =
      new C_CamMosDatabaseBusSelectionPopup(this->mc_Busses, this->mc_Database, *c_New);

   //Resize
   c_New->SetSize(QSize(700, 412));

   // Update settings on accept
   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      // remember selected bus and update data handling later on success (signal handled by database widget)
      this->mc_Database.s32_BusIndex = pc_Dialog->GetSelectedBus();
      Q_EMIT (this->SigUpdateDatabaseOsySysdefBus(this, this->mc_Database.s32_BusIndex));
   }

   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
} //lint !e429  no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for browse button.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseItemWidget::m_OnBrowse()
{
   const QString c_Name = C_CamMosDatabaseItemWidget::h_BrowseForDatabasePath(this); // multi-selection is not possible

   // do not update if path is empty
   if (c_Name != "")
   {
      bool q_Continue;
      QStringList c_Messages;
      //Ask for user confirmation, only if necessary
      C_CamProHandler::h_GetInstance()->GetAllMessagesFromDatabase(
         C_CamUti::h_GetAbsPathFromProj(this->mc_Database.c_Name), &c_Messages, NULL);
      if (c_Messages.size() > 0)
      {
         QString c_Details;
         C_OgeWiCustomMessage c_Message(this);
         QString c_Description =
            static_cast<QString>(C_GtGetText::h_GetText("Do you really want to replace this database?"));
         C_CamMosDatabaseItemWidget::h_AppendMessageWarningIfNecessary(
            C_CamUti::h_GetAbsPathFromProj(this->mc_Database.c_Name), c_Description, c_Details);
         // ask user to replace database
         c_Message.SetType(C_OgeWiCustomMessage::eWARNING);
         c_Message.SetHeading(C_GtGetText::h_GetText("Database replace"));
         c_Message.SetDescription(c_Description);
         c_Message.SetDetails(c_Details);
         c_Message.SetOkButtonText(C_GtGetText::h_GetText("Replace"));
         c_Message.SetCancelButtonText(C_GtGetText::h_GetText("Keep"));
         if (c_Message.Execute() == C_OgeWiCustomMessage::eOK)
         {
            //User confirmed
            q_Continue = true;
         }
         else
         {
            //User canceled
            q_Continue = false;
         }
      }
      else
      {
         //No need for user confirmation
         q_Continue = true;
      }
      //Replace
      if (q_Continue)
      {
         const QString c_DatabaseName =
            C_CamUtiGeneric::h_AskUserToSaveRelativePath(this, c_Name,
                                                         C_CamProHandler::h_GetInstance()->GetCurrentProjDir());

         // if path contains invalid characters this returned empty
         if (c_DatabaseName.isEmpty() == false)
         {
            this->mc_Database.c_Name = c_DatabaseName;

            // reset buses and bus index
            this->mc_Busses.clear();
            this->mc_Database.s32_BusIndex = -1;
            this->mc_FileTimeStamp = static_cast<QFileInfo>(c_Name).lastModified(); // set new timestamp

            // update GUI
            this->m_SetMinimizedPath();
            this->UpdateTooltip();

            // update data handling
            Q_EMIT (this->SigUpdateDatabasePath(this, this->mc_Database, false));
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for remove button.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseItemWidget::m_OnRemove()
{
   // inform list about remove
   Q_EMIT (this->SigRemoveDatabase(this, true));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for checkbox toggle.

   \param[in]  orq_Checked    Checked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseItemWidget::m_OnChxToggle(const bool & orq_Checked)
{
   if (!this->mq_SuppressChangeSignal)
   {
      // inform list about check box toggle
      const bool q_Result = Q_EMIT (this->SigEnableDatabase(this, orq_Checked));

      this->mc_Database.q_Enabled = q_Result;
      if (q_Result != orq_Checked)
      {
         this->mpc_Ui->pc_CheckBox->setChecked(true);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set text of label from a path.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseItemWidget::m_SetMinimizedPath(void) const
{
   const QFont c_Font = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_13);

   this->mpc_Ui->pc_LabDatabase->setText(C_Uti::h_MinimizePath(this->mc_Database.c_Name, c_Font,
                                                               this->mpc_Ui->pc_LabDatabase->width(), 66));
   // padding 66 for 3 buttons a 16px and 3x padding a 6px
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Enable tool tip of checkbox.

   \param[in]  oq_Enabled  tool tip content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseItemWidget::m_EnableCheckBoxTooltip(const bool oq_Enabled)
{
   if (oq_Enabled == true)
   {
      this->mpc_Ui->pc_CheckBox->SetToolTipInformation(
         C_GtGetText::h_GetText("Enable Database"),
         C_GtGetText::h_GetText("Check to interpret messages with this database or uncheck to remove interpretation."));
   }
   else
   {
      this->mpc_Ui->pc_CheckBox->SetToolTipInformation("", "");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for pressed signal of any button

   We need to know when a button of the item was pressed, but is not released already. The signal clicked will be
   send normally when pressed and released is sent.
   In case of an active tool tip of the button and clicking the button, the leave event C_CamMosDatabaseItemWidget will
   be caused before the released signal of the button and the group of the buttons will be set invisible
   for a short time.
   This prevents the sending of the clicked signal of the button.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseItemWidget::m_ButtonPressed(void)
{
   this->mq_ButtonPressed = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for released signal of any button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseItemWidget::m_ButtonReleased(void)
{
   this->mq_ButtonPressed = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden event function.

    Here: Show or hide buttons

   \param[in,out]  opc_Event  Event identification and information

   \retval  True   Event was recognized and processed
   \retval  False  Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMosDatabaseItemWidget::event(QEvent * const opc_Event)
{
   if (this->isEnabled() == true)
   {
      if ((opc_Event->type() == QEvent::Leave) && (this->mq_ButtonPressed == false))
      {
         // Set only invisible when no button of this item is pressed. See function m_ButtonPressed for
         // a detailed description
         this->mpc_Ui->pc_PbSelectBus->setVisible(false);
         this->mpc_Ui->pc_PbBrowse->setVisible(false);
         this->mpc_Ui->pc_PbRemove->setVisible(false);
         this->SetBackgroundColor(-1);
      }
      else if (opc_Event->type() == QEvent::Enter)
      {
         // show select bus button only if openSYDE system definition and state ok
         // (its disabled anyway but tooltip would appear)
         if ((static_cast<QFileInfo>(this->mc_Database.c_Name).suffix().compare("syde_sysdef",
                                                                                Qt::CaseInsensitive) == 0) &&
             (this->me_State == eOK))
         {
            this->mpc_Ui->pc_PbSelectBus->setVisible(true);
         }

         // show browse and remove button only if not loading (they are disabled anyway but tooltips would appear)
         if (this->me_State != eLOADING)
         {
            this->mpc_Ui->pc_PbBrowse->setVisible(true);
            this->mpc_Ui->pc_PbRemove->setVisible(true);
         }

         this->SetBackgroundColor(3);
      }
      else
      {
         // nothing to do
      }
   }

   // update text (minimized path depends on label size)
   if (opc_Event->type() == QEvent::Resize)
   {
      this->m_SetMinimizedPath();
   }

   return C_OgeWiOnlyBackground::event(opc_Event);
}
