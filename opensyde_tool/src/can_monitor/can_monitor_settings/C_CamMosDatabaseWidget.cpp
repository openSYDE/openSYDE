//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for configuring database files like DBC or syde_sysdef files.
 (implementation)

   Widget for configuring database files like DBC or syde_sysdef files.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QTimer>

#include "C_CamMosDatabaseWidget.hpp"
#include "ui_C_CamMosDatabaseWidget.h"

#include "stwtypes.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include "TglUtils.hpp"
#include "C_CamProHandler.hpp"
#include "stwerrors.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_CamMosDatabaseBusSelectionPopup.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_UsHandler.hpp"
#include "C_CamUti.hpp"
#include "C_CamUtiGeneric.hpp"
#include "constants.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;
using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosDatabaseWidget::C_CamMosDatabaseWidget(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_CamMosDatabaseWidget)
{
   this->mpc_Ui->setupUi(this);

   // initialize GUI elements
   this->m_InitUi();

   // cyclic file check configuration
   connect(&this->mc_Timer, &QTimer::timeout, this, &C_CamMosDatabaseWidget::m_OnFileCheckTimeout);
   this->m_StartCheckTimer(true);

   // load configuration
   connect(C_CamProHandler::h_GetInstance(), &C_CamProHandler::SigNewConfiguration,
           this, &C_CamMosDatabaseWidget::m_LoadConfig);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosDatabaseWidget::~C_CamMosDatabaseWidget()
{
   // clean up item widgets
   this->Clear();

   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load all user settings.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::LoadUserSettings(void) const
{
   this->mpc_Ui->pc_WiHeader->SetExpanded(C_UsHandler::h_GetInstance()->GetWiDatabaseExpanded());
   this->m_OnExpand(C_UsHandler::h_GetInstance()->GetWiDatabaseExpanded());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for load finished signal in DBC file case.

   This always correlates to first entry in list.

   \param[in]  os32_Result    Result of DBC file load
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::OnLoadFinishedDbc(const int32_t os32_Result)
{
   // check if first database in queue is of type dbc
   if (this->mc_DatabasesToLoad.isEmpty() == false)
   {
      if (this->mc_DatabasesToLoad[0] != NULL)
      {
         const C_CamProDatabaseData c_Database = mc_DatabasesToLoad[0]->GetDatabaseData();
         tgl_assert(c_Database.c_Name.endsWith(".dbc", Qt::CaseInsensitive) == true);
      }
   }

   this->m_LoadFinishedGeneric(os32_Result);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for load finished signal in openSYDE system definition case.

   This always correlates to first entry in list.

   \param[in]  os32_Result    Result of openSYDE system definition file load
   \param[in]  orc_Busses     Buses found in system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::OnLoadFinishedOsySysDef(const int32_t os32_Result,
                                                     const std::vector<C_OscSystemBus> & orc_Busses)
{
   // update database (first database in queue)
   if (this->mc_DatabasesToLoad.isEmpty() == false)
   {
      if (this->mc_DatabasesToLoad[0] != NULL)
      {
         tgl_assert(this->mc_DatabasesToLoad[0]->GetDatabaseData().c_Name.endsWith(".syde_sysdef",
                                                                                   Qt::CaseInsensitive) == true);

         // remember buses
         this->mc_DatabasesToLoad[0]->SetBusses(orc_Busses);

         // pop up bus selection if result is C_WARN (bus not found) or bus index was never set (bus index -1)
         if ((os32_Result == C_WARN) ||
             ((os32_Result == C_NO_ERR) && (this->mc_DatabasesToLoad[0]->GetDatabaseData().s32_BusIndex < 0)))
         {
            if (orc_Busses.size() > 1)
            {
               // Pop up dialog (use scroll area widget as parent to make sure scroll bars are styled correctly)
               const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this->mpc_Ui->pc_ScrollAreaContents,
                                                                             this->mpc_Ui->pc_ScrollAreaContents);

               C_CamMosDatabaseBusSelectionPopup * const pc_Dialog =
                  new C_CamMosDatabaseBusSelectionPopup(orc_Busses, this->mc_DatabasesToLoad[0]->GetDatabaseData(),
                                                        *c_New);

               //Resize
               const QSize c_SIZE(700, 412);
               c_New->SetSize(c_SIZE);

               // Execute
               if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
               {
                  // remember selected bus and update data handling later on success (signal)
                  this->mc_DatabasesToLoad[0]->SetBusIndex(pc_Dialog->GetSelectedBus());

                  // inform which bus to use
                  this->m_UpdateDatabaseOsySysDefBus(this->mc_DatabasesToLoad[0], pc_Dialog->GetSelectedBus());
               }
               else
               {
                  // delete database if dialog was canceled
                  this->m_RemoveDatabase(mc_DatabasesToLoad[0], false);
               }

               if (c_New != NULL)
               {
                  c_New->HideOverlay();
               }
            } //lint !e429  no memory leak because of the parent pc_Dialog and the Qt memory management
         }
         this->m_LoadFinishedGeneric(os32_Result);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for result of openSYDE bus selection.

   \param[in]  orc_PathSystemDefinition   system definition identifier (i.e. path)
   \param[in]  os32_Result                result of bus selection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::OnSigOsySysDefBusResult(const QString & orc_PathSystemDefinition,
                                                     const int32_t os32_Result)
{
   C_CamMosDatabaseItemWidget * pc_Item = NULL;

   // search for item widget with given path
   for (std::vector<C_CamMosDatabaseItemWidget *>::const_iterator c_It = mc_Entries.begin(); c_It != mc_Entries.end();
        ++c_It)
   {
      if (*c_It != NULL)
      {
         pc_Item = *c_It;
         if (C_CamUti::h_GetAbsPathFromProj(pc_Item->GetDatabaseData().c_Name) == orc_PathSystemDefinition)
         {
            break;
         }
         else
         {
            pc_Item = NULL;
         }
      }
   }

   // distinguish no error, warning and rest
   if (os32_Result == C_NO_ERR)
   {
      if (pc_Item != NULL)
      {
         // update data handling
         C_CamProHandler::h_GetInstance()->SetDatabaseOsySysDefBus(this->m_GetIndexFromWidget(pc_Item),
                                                                   pc_Item->GetDatabaseData().s32_BusIndex);
      }
   }
   else if (os32_Result == C_WARN)
   {
      // reset bus index to previous one
      if (pc_Item != NULL)
      {
         pc_Item->SetBusIndex(
            C_CamProHandler::h_GetInstance()->GetDatabases()[this->m_GetIndexFromWidget(pc_Item)].s32_BusIndex);
      }
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eWARNING);
      c_Message.SetHeading(C_GtGetText::h_GetText("Bus Selection"));
      c_Message.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                       "An error occurred on bus that was selected for database %1. "
                                                       "Returned to previous bus.")).arg(
                                  orc_PathSystemDefinition));
      c_Message.Execute();
   }
   else
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);

      c_Message.SetHeading(C_GtGetText::h_GetText("Bus Selection"));
      c_Message.SetDescription(static_cast<QString>(
                                  C_GtGetText::h_GetText(
                                     "An unknown error occurred on bus that was selected for database %1. "
                                     "Behavior of used bus undefined and can be displayed wrong.")).
                               arg(orc_PathSystemDefinition));
      c_Message.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for communication change signal.

   \param[in]  oq_Online   Online/offline flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::OnCommunicationStarted(const bool oq_Online)
{
   this->m_StartCheckTimer(!oq_Online);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Prepare widget for popup view in collapsed state resp. expanded view.

   \param[in]  oq_Expand   true: prepare for expanded, false: prepare for collapsed popup
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::PrepareForExpanded(const bool oq_Expand) const
{
   if (oq_Expand == false)
   {
      this->mpc_Ui->pc_WiContent->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_WiContent->setVisible(C_UsHandler::h_GetInstance()->GetWiDatabaseExpanded());
   }

   // hide arrow button
   this->mpc_Ui->pc_WiHeader->ShowExpandButton(oq_Expand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update all paths if save location changed.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::OnSigSavedAsNew(void) const
{
   C_CamMosDatabaseItemWidget * pc_Item = NULL;
   const std::vector<C_CamProDatabaseData> c_Databases = C_CamProHandler::h_GetInstance()->GetDatabases();

   // load new database paths
   uint32_t u32_It = 0;

   for (std::vector<C_CamMosDatabaseItemWidget *>::const_iterator c_It = mc_Entries.begin(); c_It != mc_Entries.end();
        ++c_It)
   {
      tgl_assert((*c_It != NULL) && (u32_It < c_Databases.size()));
      if ((*c_It != NULL) && (u32_It < c_Databases.size()))
      {
         const C_CamProDatabaseData & rc_Database = c_Databases[u32_It];
         pc_Item = *c_It;
         pc_Item->UpdateDatabasePathRelativeness(rc_Database.c_Name);
      }

      ++u32_It;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add dropped database. Forwarded from main window.

   \param[in]  orc_Path    path of database file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::AddDroppedDatabase(const QString & orc_Path)
{
   // if necessary adapt .syde path to .syde_sysdef
   const QFileInfo c_FileInfo = static_cast<QFileInfo>(orc_Path); // dropped paths are always absolute so no problems
   const QString c_Path = C_CamMosDatabaseItemWidget::h_AdaptPathToSystemDefinitionIfNecessary(c_FileInfo);

   // check if already used
   if (C_CamMosDatabaseItemWidget::h_IsDatabaseAlreadyUsed(c_Path, this) == false)
   {
      C_CamProDatabaseData c_Database;

      const QString c_DatabaseName =
         C_CamUtiGeneric::h_AskUserToSaveRelativePath(this, c_Path,
                                                      C_CamProHandler::h_GetInstance()->GetCurrentProjDir());

      // if path contains invalid characters this returned empty
      if (c_DatabaseName.isEmpty() == false)
      {
         c_Database.c_Name = c_DatabaseName;
         // add in data handling
         C_CamProHandler::h_GetInstance()->AddDatabase(c_Database);

         // add new widget (GUI)
         this->m_AddDatabaseWidget(c_Database);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::Clear(void)
{
   // clear loading queue (first to avoid ghost signals)
   mc_DatabasesToLoad.clear();

   for (uint32_t u32_ItEntry = 0; u32_ItEntry < this->mc_Entries.size(); ++u32_ItEntry)
   {
      // remove database from interpreting
      Q_EMIT (this->SigRemoveDatabase(
                 C_CamUti::h_GetAbsPathFromProj(this->mc_Entries[u32_ItEntry]->GetDatabaseData().c_Name),
                 this->mc_Entries[u32_ItEntry]->GetDatabaseData().c_Name, false));

      // delete widget from GUI
      this->m_RemoveDatabaseWidget(this->mc_Entries[u32_ItEntry]);
   }

   // clear list of database widgets
   mc_Entries.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize GUI elements
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::m_InitUi(void)
{
   // initialize colors
   this->SetBackgroundColor(5);

   this->mpc_Ui->pc_ScrollAreaContents->SetBackgroundColor(5);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameBottom, "HasColor8Background", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameTop, "HasColor8Background", true);

   // initialize title widget
   this->mpc_Ui->pc_WiHeader->SetTitle(C_GtGetText::h_GetText("Database"));
   this->mpc_Ui->pc_WiHeader->SetIcon("://images/IconDatabase.svg");
   this->mpc_Ui->pc_WiHeader->SetToggle(false);

   // initialize add button
   QIcon c_Icon;
   c_Icon.addFile("://images/IconAdd.svg", QSize(), QIcon::Normal);
   c_Icon.addFile("://images/IconAddDisabled.svg", QSize(), QIcon::Disabled);
   this->mpc_Ui->pc_BtnAdd->setIconSize(QSize(11, 11));
   this->mpc_Ui->pc_BtnAdd->setIcon(c_Icon);
   this->mpc_Ui->pc_BtnAdd->setText(C_GtGetText::h_GetText("Add Database"));
   this->mpc_Ui->pc_BtnAdd->SetToolTipInformation(C_GtGetText::h_GetText("Add New Database"),
                                                  C_GtGetText::h_GetText(
                                                     "Add new database (*.syde or *.dbc) from file system."));

   // initialize label
   this->mpc_Ui->pc_LabNoDatabase->SetForegroundColor(0);
   this->mpc_Ui->pc_LabNoDatabase->SetFontPixel(12);
   this->mpc_Ui->pc_LabNoDatabase->setEnabled(false);

   this->mpc_Ui->pc_LabNoDatabase->setText(
      C_GtGetText::h_GetText("No databases are declared.\nAdd any via \"Add Database\"."));

   connect(this->mpc_Ui->pc_WiHeader, &C_CamOgeWiSettingSubSection::SigExpandSection,
           this, &C_CamMosDatabaseWidget::m_OnExpand);
   connect(this->mpc_Ui->pc_WiHeader, &C_CamOgeWiSettingSubSection::SigHide,
           this, &C_CamMosDatabaseWidget::SigHide);
   connect(this->mpc_Ui->pc_BtnAdd, &C_CamOgePubSettingsAdd::clicked, this, &C_CamMosDatabaseWidget::m_OnAddClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load configuration (i.e. database widgets)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::m_LoadConfig(void)
{
   // show no database label as default
   this->mpc_Ui->pc_GroupBoxNoElements->setVisible(true);

   // initialize Database widgets
   std::vector<C_CamProDatabaseData> c_Databases = C_CamProHandler::h_GetInstance()->GetDatabases();

   for (uint32_t u32_ItDatabase = 0UL; u32_ItDatabase < c_Databases.size(); ++u32_ItDatabase)
   {
      this->m_AddDatabaseWidget(c_Databases[u32_ItDatabase]);
   }

   // update title
   this->m_UpdateTitleDatabaseCount();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove database widget (GUI only)

   Counterpart of m_AddDatabaseWidget.
   Does no external data handling.

   \param[in]  opc_ItemWidget    Database widget to remove
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::m_RemoveDatabaseWidget(C_CamMosDatabaseItemWidget * const opc_ItemWidget) const
{
   if (opc_ItemWidget != NULL)
   {
      // disconnect everything
      opc_ItemWidget->disconnect();

      // remove widget from layout
      this->mpc_Ui->pc_LayoutContents->removeWidget(opc_ItemWidget);

      // delete widget
      delete (opc_ItemWidget);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for add button.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::m_OnAddClicked()
{
   const QString c_Name = C_CamMosDatabaseItemWidget::h_BrowseForDatabasePath(this);
   C_CamProDatabaseData c_Database;

   // do not add if path is empty
   if (c_Name != "")
   {
      const QString c_DatabaseName =
         C_CamUtiGeneric::h_AskUserToSaveRelativePath(this, c_Name,
                                                      C_CamProHandler::h_GetInstance()->GetCurrentProjDir());

      // if path contains invalid characters this returned empty
      if (c_DatabaseName.isEmpty() == false)
      {
         c_Database.c_Name = c_DatabaseName;

         // add in data handling
         C_CamProHandler::h_GetInstance()->AddDatabase(c_Database);

         // add new widget (GUI)
         this->m_AddDatabaseWidget(c_Database);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new database widget(GUI only).

   Counterpart of m_RemoveDatabaseWidget.
   Does no external data handling.

   \param[in]  orc_DatabaseData  database data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::m_AddDatabaseWidget(const C_CamProDatabaseData & orc_DatabaseData)
{
   C_CamMosDatabaseItemWidget * const pc_NewItem = new C_CamMosDatabaseItemWidget(orc_DatabaseData, this);

   // remember item
   this->mc_Entries.push_back(pc_NewItem);

   // insert new item before spacer -> "-1"
   this->mpc_Ui->pc_LayoutContents->insertWidget(this->mpc_Ui->pc_LayoutContents->count() - 1, pc_NewItem);

   // hide "no database" label
   this->mpc_Ui->pc_GroupBoxNoElements->setVisible(false);

   // update count in title
   this->m_UpdateTitleDatabaseCount();

   // add to loading queue
   this->m_AddToLoadingQueue(pc_NewItem);

   // connect
   connect(pc_NewItem, &C_CamMosDatabaseItemWidget::SigRemoveDatabase,
           this, &C_CamMosDatabaseWidget::m_RemoveDatabase);
   connect(pc_NewItem, &C_CamMosDatabaseItemWidget::SigEnableDatabase,
           this, &C_CamMosDatabaseWidget::m_ActivateDatabase);
   connect(pc_NewItem, &C_CamMosDatabaseItemWidget::SigUpdateDatabasePath,
           this, &C_CamMosDatabaseWidget::m_UpdateDatabasePath);
   connect(pc_NewItem, &C_CamMosDatabaseItemWidget::SigUpdateDatabaseOsySysdefBus,
           this, &C_CamMosDatabaseWidget::m_UpdateDatabaseOsySysDefBus);
   connect(pc_NewItem, &C_CamMosDatabaseItemWidget::SigNotifyMissingDataBase,
           this, &C_CamMosDatabaseWidget::SigNotifyMissingDataBase);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove existing database (widget and data).

   \param[in,out]  opc_ItemWidget   Item widget
   \param[in]      oq_AskUser       Ask user
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::m_RemoveDatabase(C_CamMosDatabaseItemWidget * const opc_ItemWidget, const bool oq_AskUser)
{
   bool q_Continue = true;

   if (oq_AskUser == true)
   {
      QString c_Details;
      QString c_Description = C_GtGetText::h_GetText("Do you really want to delete this database?");
      // ask user
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eQUESTION);

      C_CamMosDatabaseItemWidget::h_AppendMessageWarningIfNecessary(
         opc_ItemWidget->GetDatabaseData().c_Name, c_Description, c_Details);
      c_Message.SetHeading(C_GtGetText::h_GetText("Database Delete"));
      c_Message.SetDescription(c_Description);
      c_Message.SetDetails(c_Details);
      c_Message.SetOkButtonText(C_GtGetText::h_GetText("Delete"));
      c_Message.SetNoButtonText(C_GtGetText::h_GetText("Keep"));

      if (c_Message.Execute() != C_OgeWiCustomMessage::eOK)
      {
         q_Continue = false;
      }
   }

   if ((q_Continue == true) && (opc_ItemWidget != NULL))
   {
      int32_t s32_IndexToRemove = 0;

      // remove widget from list
      for (std::vector<C_CamMosDatabaseItemWidget *>::iterator c_It = mc_Entries.begin(); c_It != mc_Entries.end();
           ++c_It)
      {
         if (*c_It == opc_ItemWidget)
         {
            this->mc_Entries.erase(c_It);

            break;
         }
         s32_IndexToRemove++;
      }

      // remove widget from queue
      this->mc_DatabasesToLoad.removeOne(opc_ItemWidget);

      // remove database
      Q_EMIT (this->SigRemoveDatabase(C_CamUti::h_GetAbsPathFromProj(opc_ItemWidget->GetDatabaseData().c_Name),
                                      opc_ItemWidget->GetDatabaseData().c_Name, false));

      // delete widget from GUI
      this->m_RemoveDatabaseWidget(opc_ItemWidget);

      // update data handling
      tgl_assert(C_CamProHandler::h_GetInstance()->DeleteDatabase(s32_IndexToRemove) == C_NO_ERR);
   }

   // number of database widgets should now equal number of filters in data handling
   tgl_assert(C_CamProHandler::h_GetInstance()->GetDatabases().size() == mc_Entries.size());

   // eventually show "no database" label
   if (this->mc_Entries.size() == 0)
   {
      this->mpc_Ui->pc_GroupBoxNoElements->setVisible(true);
   }

   // update count in title
   this->m_UpdateTitleDatabaseCount();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Activate or deactivate given filter.

   \param[in]  opc_ItemWidget    Filter widget to select
   \param[in]  orq_Enable        True: activate filter, false: deactivate filter

   \return
   Resulting state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMosDatabaseWidget::m_ActivateDatabase(const C_CamMosDatabaseItemWidget * const opc_ItemWidget,
                                                const bool & orq_Enable)
{
   bool q_State = orq_Enable;
   const int32_t s32_Index = this->m_GetIndexFromWidget(opc_ItemWidget);
   bool q_Continue = true;

   if ((orq_Enable == false) && (opc_ItemWidget != NULL))
   {
      QString c_Details;
      QString c_TmpDescription;
      // ask user
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eQUESTION);

      C_CamMosDatabaseItemWidget::h_AppendMessageWarningIfNecessary(
         opc_ItemWidget->GetDatabaseData().c_Name, c_TmpDescription, c_Details);
      if (c_TmpDescription.isEmpty() == false)
      {
         QString c_Description = C_GtGetText::h_GetText("Do you really want to disable this database?");
         c_Description.append(c_TmpDescription);
         c_Message.SetHeading(C_GtGetText::h_GetText("Database Disable"));
         c_Message.SetDescription(c_Description);
         c_Message.SetDetails(c_Details);
         c_Message.SetOkButtonText(C_GtGetText::h_GetText("Disable"));
         c_Message.SetNoButtonText(C_GtGetText::h_GetText("Keep"));

         if (c_Message.Execute() != C_OgeWiCustomMessage::eOK)
         {
            q_Continue = false;
            q_State = !orq_Enable;
         }
      }
   }
   if (q_Continue)
   {
      // index is -1 if item widget is NULL which can not happen
      tgl_assert(s32_Index >= 0);
      if (s32_Index >= 0)
      {
         C_CamProHandler::h_GetInstance()->SetDatabaseEnabled(s32_Index, orq_Enable);
      }

      // activate database
      // if database is not yet loaded this is a no-op and the signal gets emitted again as soon as load finished
      if (opc_ItemWidget != NULL)
      {
         Q_EMIT (this->SigActivateDatabase(
                    C_CamUti::h_GetAbsPathFromProj(opc_ItemWidget->GetDatabaseData().c_Name),
                    opc_ItemWidget->GetDatabaseData().c_Name, orq_Enable));
      }
   }
   return q_State;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update database configuration of specified database.

   Slot for signal from item widget on browse finished.

   \param[in]  opc_ItemWidget    widget of database item
   \param[in]  orc_Database      database data
   \param[in]  oq_IsUpdate       Flag if action is used to update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::m_UpdateDatabasePath(C_CamMosDatabaseItemWidget * const opc_ItemWidget,
                                                  const C_CamProDatabaseData & orc_Database, const bool oq_IsUpdate)
{
   const int32_t s32_Index = this->m_GetIndexFromWidget(opc_ItemWidget);
   const std::vector<C_CamProDatabaseData>  & rc_Databases =  C_CamProHandler::h_GetInstance()->GetDatabases();

   // index is -1 if item widget is NULL which can not happen
   tgl_assert((s32_Index >= 0) && (s32_Index < static_cast<int32_t>(rc_Databases.size())));
   if ((s32_Index >= 0) && (s32_Index < static_cast<int32_t>(rc_Databases.size())))
   {
      // remove deprecated database
      Q_EMIT (this->SigRemoveDatabase(C_CamUti::h_GetAbsPathFromProj(rc_Databases[s32_Index].c_Name),
                                      rc_Databases[s32_Index].c_Name, oq_IsUpdate));

      // set new name
      C_CamProHandler::h_GetInstance()->SetDatabaseName(s32_Index, orc_Database.c_Name);

      // also update bus index
      C_CamProHandler::h_GetInstance()->SetDatabaseOsySysDefBus(s32_Index, orc_Database.s32_BusIndex);

      this->m_AddToLoadingQueue(opc_ItemWidget);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update database bus index of specified database (of type openSYDE system definition).

   Slot for bus editing finished.

   \param[in]  opc_ItemWidget    widget of database item
   \param[in]  os32_BusIndex     bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::m_UpdateDatabaseOsySysDefBus(const C_CamMosDatabaseItemWidget * const opc_ItemWidget,
                                                          const int32_t os32_BusIndex)
{
   Q_EMIT (this->SigSetDatabaseOsySysDefBus(C_CamUti::h_GetAbsPathFromProj(opc_ItemWidget->GetDatabaseData().c_Name),
                                            opc_ItemWidget->GetDatabaseData().c_Name,
                                            os32_BusIndex));
   // note: data handling is done later after another signal was received
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data handling index of database from database widget pointer.

   \param[in]  opc_ItemWidget    pointer to database widget

   \return
   index of database in data handling
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamMosDatabaseWidget::m_GetIndexFromWidget(const C_CamMosDatabaseItemWidget * const opc_ItemWidget)
{
   int32_t s32_Return = 0;

   if (opc_ItemWidget != NULL)
   {
      // get index from opc_Item
      for (std::vector<C_CamMosDatabaseItemWidget *>::iterator c_It = mc_Entries.begin(); c_It != mc_Entries.end();
           ++c_It)
      {
         if (*c_It == opc_ItemWidget)
         {
            break;
         }
         s32_Return++;
      }
   }
   else
   {
      s32_Return = -1;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add database to loading queue and trigger loading start if queue was empty before.

   \param[in]  opc_ItemWidget    pointer to database widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::m_AddToLoadingQueue(C_CamMosDatabaseItemWidget * const opc_ItemWidget)
{
   if (opc_ItemWidget != NULL)
   {
      this->mc_DatabasesToLoad.append(opc_ItemWidget);

      opc_ItemWidget->SetState(C_CamMosDatabaseItemWidget::eQUEUE);

      // start database loading if list was empty
      if (this->mc_DatabasesToLoad.size() == 1)
      {
         this->m_CheckAndLoadDatabase();
      }
   }
} //lint !e429  no memory leak because of remove handling of items

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Start loading next database.

   Check database type and emit associated signal to start loading this database.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::m_CheckAndLoadDatabase(void)
{
   if (this->mc_DatabasesToLoad.isEmpty() == false)
   {
      if (mc_DatabasesToLoad[0] != NULL)
      {
         // load first database from loading queue
         const C_CamProDatabaseData c_Database = mc_DatabasesToLoad[0]->GetDatabaseData();
         // check if file exists
         const QFileInfo c_DatabaseFile(C_CamUti::h_GetAbsPathFromProj(c_Database.c_Name));
         if (c_DatabaseFile.exists() == true)
         {
            // check suffix to distinguish dbc, osy system definitions or invalid suffix
            if (c_DatabaseFile.suffix().compare("dbc", Qt::CaseInsensitive) == 0)
            {
               this->mc_DatabasesToLoad[0]->SetState(C_CamMosDatabaseItemWidget::eLOADING);
               Q_EMIT (this->SigAddDatabaseDbc(C_CamUti::h_GetAbsPathFromProj(c_Database.c_Name), c_Database.c_Name));
            }
            else if (c_DatabaseFile.suffix().compare("syde_sysdef", Qt::CaseInsensitive) == 0)
            {
               this->mc_DatabasesToLoad[0]->SetState(C_CamMosDatabaseItemWidget::eLOADING);
               Q_EMIT (this->SigAddDatabaseOsySysDef(C_CamUti::h_GetAbsPathFromProj(c_Database.c_Name),
                                                     c_Database.c_Name,
                                                     c_Database.s32_BusIndex));
            }
            else
            {
               this->mc_DatabasesToLoad[0]->SetState(C_CamMosDatabaseItemWidget::eWRONG_FORMAT);
               // remove from queue and start next
               this->mc_DatabasesToLoad.removeFirst();
               this->m_CheckAndLoadDatabase();
            }
         }
         else
         {
            this->mc_DatabasesToLoad[0]->SetState(C_CamMosDatabaseItemWidget::eNOT_FOUND);
            // remove from queue and start next
            this->mc_DatabasesToLoad.removeFirst();
            this->m_CheckAndLoadDatabase();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update loading state of database and start loading next one.

   Generic because it does not depend on database type.

   \param[in]  os32_ErrorCode    error code of loading
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::m_LoadFinishedGeneric(const int32_t os32_ErrorCode)
{
   // there is at least one entry in databases loading queue because we requested load for exactly this database
   if (this->mc_DatabasesToLoad.isEmpty() == false)
   {
      if (this->mc_DatabasesToLoad[0] != NULL)
      {
         const C_CamProDatabaseData c_Database = mc_DatabasesToLoad[0]->GetDatabaseData();

         // adapt icon of actual database: load success/failed (also accept C_WARN because no big issues occured)
         if ((os32_ErrorCode == C_NO_ERR) || (os32_ErrorCode == C_WARN))
         {
            this->mc_DatabasesToLoad[0]->SetState(C_CamMosDatabaseItemWidget::eOK);
            this->mc_DatabasesToLoad[0]->UpdateTooltip();

            // send activate signal
            Q_EMIT (this->SigActivateDatabase(C_CamUti::h_GetAbsPathFromProj(c_Database.c_Name), c_Database.c_Name,
                                              c_Database.q_Enabled));
         }
         else
         {
            this->mc_DatabasesToLoad[0]->SetState(C_CamMosDatabaseItemWidget::eLOAD_ERROR);
         }

         // at least (!) remove entry from list
         this->mc_DatabasesToLoad.removeFirst();
      }
   }
   // if there is another database in list: start loading
   if (this->mc_DatabasesToLoad.isEmpty() == false)
   {
      this->m_CheckAndLoadDatabase();
   }

   // remaining databases in list: in queue (no changes)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for file check timer timeout.

   Check all files for existence and changes.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::m_OnFileCheckTimeout()
{
   // check all files
   for (uint32_t u32_Pos = 0; u32_Pos < this->mc_Entries.size(); u32_Pos++)
   {
      if (this->mc_Entries[u32_Pos] != NULL)
      {
         C_CamMosDatabaseItemWidget & rc_Item = *this->mc_Entries[u32_Pos];
         rc_Item.CheckFile();
      }
   }

   // time till next check can be greater than time till first check
   this->mc_Timer.setInterval(mu32_DATABASE_CHECK_INTERVAL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Start or stop file check timer.

   \param[in]  oq_Active   true: start timer, false: stop timer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::m_StartCheckTimer(const bool oq_Active)
{
   if (oq_Active == true)
   {
      this->mc_Timer.start(25);
   }
   else
   {
      this->mc_Timer.stop();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of expand signal.

   \param[in]  oq_Expand   true: expand; false: collapse
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::m_OnExpand(const bool oq_Expand) const
{
   this->mpc_Ui->pc_WiContent->setVisible(oq_Expand);
   C_UsHandler::h_GetInstance()->SetWiDatabaseExpanded(oq_Expand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update title: show number of databases
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseWidget::m_UpdateTitleDatabaseCount(void) const
{
   this->mpc_Ui->pc_WiHeader->SetTitle(
      static_cast<QString>(C_GtGetText::h_GetText("Database (%1)")).arg(this->mc_Entries.size()));
}
