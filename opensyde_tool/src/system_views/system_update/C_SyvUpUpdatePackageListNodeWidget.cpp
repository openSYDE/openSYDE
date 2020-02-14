//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific node with its applications and data blocks as list item

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QFileInfo>
#include <QScrollBar>

#include "C_SyvUpUpdatePackageListNodeWidget.h"
#include "ui_C_SyvUpUpdatePackageListNodeWidget.h"

#include "stwerrors.h"
#include "constants.h"

#include "C_Uti.h"
#include "C_OgeWiUtil.h"

#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_OSCNode.h"
#include "C_PuiProject.h"
#include "C_PuiSvData.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_OSCLoggingHandler.h"
#include "C_OsyHexFile.h"
#include "C_OgeWiCustomMessage.h"
#include "C_ImpUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;

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
C_SyvUpUpdatePackageListNodeWidget::C_SyvUpUpdatePackageListNodeWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpUpdatePackageListNodeWidget),
   mu32_ViewIndex(0U),
   mu32_NodeIndex(0U),
   mc_NodeName(""),
   mc_DeviceType(""),
   mq_FileBased(false),
   mq_StwFlashloader(false),
   mu32_FileCount(0U),
   mq_ShowAddButton(false),
   mu32_PositionNumber(0U),
   mq_Connected(false)
{
   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_LabTitle->SetFontPixel(14, true, false);
   this->mpc_Ui->pc_LabCount->SetFontPixel(14, true, false);
   this->mpc_Ui->pc_LabNoFiles->SetFontPixel(13, false, false);
   this->mpc_Ui->pc_LabTitle->SetForegroundColor(6);
   this->mpc_Ui->pc_LabCount->SetForegroundColor(6);
   this->mpc_Ui->pc_LabNoFiles->SetForegroundColor(8);
   this->mpc_Ui->pc_ListTitleWidget->SetBackgroundColor(11);

   this->mpc_Ui->pc_WidgetAdd->setVisible(false);

   // expanded = checked = arrow down (arrow implies actual state); collapsed = unchecked = arrow right
   this->mpc_Ui->pc_PbExpColl->setCheckable(true);
   this->mpc_Ui->pc_PbExpColl->setChecked(true);
   this->mpc_Ui->pc_PbExpColl->SetSvg("://images/system_views/IconArrowRight.svg", "", "",
                                      "://images/system_views/IconArrowDown.svg");

   connect(this->mpc_Ui->pc_PbExpColl, &C_OgePubSvgIconOnly::toggled, this,
           &C_SyvUpUpdatePackageListNodeWidget::m_OnExpand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpUpdatePackageListNodeWidget::~C_SyvUpUpdatePackageListNodeWidget()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of the widget

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_PositionNumber  Position number of node for showing
   \param[in]  ou32_NodeIndex       Node index
   \param[in]  orc_NodeName         Name of the node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::InitWidget(const uint32 ou32_ViewIndex, const uint32 ou32_PositionNumber,
                                                    const uint32 ou32_NodeIndex, const QString & orc_NodeName)
{
   this->mu32_ViewIndex = ou32_ViewIndex;
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mc_NodeName = orc_NodeName;
   this->mu32_PositionNumber = ou32_PositionNumber;

   // Load applications
   this->m_InitItems();

   this->InitStaticNames();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabNoFiles->setText(C_GtGetText::h_GetText(
                                           "No files.\nAdd any via the context menu or drag and drop."));

   this->m_InitStaticNames();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget for the connected state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::SetConnected(void)
{
   uint32 u32_Counter;

   this->mq_Connected = true;
   this->mpc_Ui->pc_WidgetAdd->setEnabled(false);

   for (u32_Counter = 0U; u32_Counter < this->mu32_FileCount; ++u32_Counter)
   {
      this->m_SetApplicationConnected(u32_Counter, true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the application update was started
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::SetUpdateApplicationStarted(void) const
{
   this->m_SetApplicationState(this->m_GetFirstNotFinishedApplication(),
                               C_SyvUpUpdatePackageListNodeItemWidget::hu32_STATE_IN_PROGRESS);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the updated was started
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::SetUpdateApplicationFinished(void) const
{
   this->m_SetApplicationState(
      this->m_GetFirstNotFinishedApplication(), C_SyvUpUpdatePackageListNodeItemWidget::hu32_STATE_FINISHED);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the application update hat an error
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::SetUpdateApplicationError(void) const
{
   this->m_SetApplicationState(
      this->m_GetFirstNotFinishedApplication(), C_SyvUpUpdatePackageListNodeItemWidget::hu32_STATE_ERROR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the entire update was finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::SetUpdateFinished(void) const
{
   if (this->mq_FileBased == true)
   {
      uint32 u32_Counter;

      // Special case: After updating a file based node, it is not possible to read back or confirm
      // the state of the node. Reset the state of all applications
      for (u32_Counter = 0U; u32_Counter < this->mu32_FileCount; ++u32_Counter)
      {
         this->m_SetApplicationState(u32_Counter, C_SyvUpUpdatePackageListNodeItemWidget::hu32_STATE_TO_DO);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger device application information discard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::DiscardApplicationStatus(void) const
{
   for (uint32 u32_Counter = 0U; u32_Counter < this->mu32_FileCount; ++u32_Counter)
   {
      this->m_SetApplicationState(u32_Counter, C_SyvUpUpdatePackageListNodeItemWidget::hu32_STATE_TO_DO);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget for the disconnected state

   Reset the application states
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::SetDisconnected(void)
{
   uint32 u32_Counter;

   this->mq_Connected = false;

   for (u32_Counter = 0U; u32_Counter < this->mu32_FileCount; ++u32_Counter)
   {
      this->m_SetApplicationState(u32_Counter, C_SyvUpUpdatePackageListNodeItemWidget::hu32_STATE_DEFAULT);
      this->m_SetApplicationConnected(u32_Counter, false);
   }

   this->mpc_Ui->pc_WidgetAdd->setEnabled(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the position number

   \param[in]  ou32_PositionNumber  New position number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::UpdatePositionNumber(const uint32 ou32_PositionNumber)
{
   this->mu32_PositionNumber = ou32_PositionNumber;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new file for the file container

   Default implementation can not add new files

   \param[in]  orc_File    New path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::AddFile(const QString & orc_File)
{
   Q_UNUSED(orc_File)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new file for the application

   \param[in]  orc_File    New path
   \param[in]  opc_App     Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::AdaptFile(const QString & orc_File,
                                                   C_SyvUpUpdatePackageListNodeItemWidget * const opc_App)
{
   if (opc_App != NULL)
   {
      // Adapt the widget itself
      opc_App->SetAppFile(orc_File, false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Revert the current application to the system definition default path

   Default implementation can not revert files

   \param[in]  opc_App  Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::RevertFile(C_SyvUpUpdatePackageListNodeItemWidget * const opc_App)
{
   Q_UNUSED(opc_App)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes the current application

   \param[in]  opc_App  Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::RemoveFile(C_SyvUpUpdatePackageListNodeItemWidget * const opc_App)
{
   if (opc_App != NULL)
   {
      // Remove the widget
      this->mpc_Ui->pc_AppVerticalLayout->removeWidget(opc_App);
      opc_App->setParent(NULL);
      delete opc_App;

      this->m_UpdateNumbers();
      if (this->mu32_FileCount > 0)
      {
         --this->mu32_FileCount;
      }
      this->m_FileCountChanged();
   }

   this->mpc_Ui->pc_WidgetAdd->setVisible(this->mpc_Ui->pc_AppVerticalLayout->count() <= 1);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes all application paths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::RemoveAllFiles(void)
{
   sintn sn_Counter;

   for (sn_Counter = this->mpc_Ui->pc_AppVerticalLayout->count() - 1; sn_Counter >= 0; --sn_Counter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_Counter);

      if (pc_Item != NULL)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SyvUpUpdatePackageListNodeItemWidget * const pc_App =
            dynamic_cast<C_SyvUpUpdatePackageListNodeItemWidget *>(pc_Item->widget());

         if (pc_App != NULL)
         {
            this->RemoveFile(pc_App);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update device information

   Check and update of information which application is already finished and must not be updated
   Default implementation sets all files to hu32_STATE_TO_DO

   \param[in]  orc_DeviceInformation   Device info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::UpdateDeviceInformation(const C_SyvUpDeviceInfo & orc_DeviceInformation)
const
{
   sintn sn_AppWidgetCounter;

   Q_UNUSED(orc_DeviceInformation)

   //go through all applications in the UI
   for (sn_AppWidgetCounter = 0; sn_AppWidgetCounter < this->mpc_Ui->pc_AppVerticalLayout->count();
        ++sn_AppWidgetCounter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_AppWidgetCounter);

      if (pc_Item != NULL)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SyvUpUpdatePackageListNodeItemWidget * const pc_App =
            dynamic_cast<C_SyvUpUpdatePackageListNodeItemWidget *>(pc_Item->widget());

         if (pc_App != NULL)
         {
            pc_App->SetState(C_SyvUpUpdatePackageListNodeItemWidget::hu32_STATE_TO_DO);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks all paths for existence

   \param[out]  oru32_CountFiles             Number of files
   \param[out]  opc_MissingApps              App names of apps with missing file
   \param[out]  opc_FlashwareWarningsApps    App names of apps with flashware warnings

   \return
   C_NO_ERR    All files are existent
   C_CONFIG    Minimum one file was not found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvUpUpdatePackageListNodeWidget::CheckAllFiles(stw_types::uint32 & oru32_CountFiles,
                                                         QStringList * const opc_MissingApps,
                                                         QStringList * const opc_FlashwareWarningsApps)
{
   sintn sn_Counter = 0;
   sint32 s32_Return = C_NO_ERR;

   while (sn_Counter < this->mpc_Ui->pc_AppVerticalLayout->count())
   {
      bool q_NewItemAtCurrentPosition = false;
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_Counter);

      if (pc_Item != NULL)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SyvUpUpdatePackageListNodeItemWidget * const pc_App =
            dynamic_cast<C_SyvUpUpdatePackageListNodeItemWidget *>(pc_Item->widget());

         if (pc_App != NULL)
         {
            // Check the app path
            const sint32 s32_CheckPathResult = pc_App->CheckPath();
            // missing files
            if (s32_CheckPathResult == C_CONFIG)
            {
               if (opc_MissingApps != NULL)
               {
                  const QString c_New =
                     QString("#%1 - %2 - %3").arg(this->mu32_PositionNumber + 1U).arg(this->mc_NodeName).
                     arg(pc_App->GetAppName());
                  opc_MissingApps->push_back(c_New);
               }
               s32_Return = C_CONFIG;
            }
            // flashware warnings
            if (s32_CheckPathResult == C_WARN)
            {
               if (opc_FlashwareWarningsApps != NULL)
               {
                  QString c_New;
                  if (pc_App->IsAppInfoAmbiguous() == false)
                  {
                     c_New =
                        QString(C_GtGetText::h_GetText("#%1 - %2 - %3: Device type %4 does not match node type %5")).
                        arg(this->mu32_PositionNumber + 1U).arg(this->mc_NodeName).arg(pc_App->GetAppName()).
                        arg(pc_App->GetAppDeviceType()).arg(this->mc_DeviceType);
                  }
                  else
                  {
                     c_New =
                        QString(C_GtGetText::h_GetText("#%1 - %2 - %3: HEX file has multiple application blocks.")).
                        arg(this->mu32_PositionNumber + 1U).arg(this->mc_NodeName).arg(pc_App->GetAppName());
                  }
                  opc_FlashwareWarningsApps->push_back(c_New);
               }
            }
            // trigger remove
            if (s32_CheckPathResult == C_TIMEOUT)
            {
               this->RemoveFile(pc_App);
               q_NewItemAtCurrentPosition = true;
            }
            else
            {
               //Only count file if file is kept
               ++oru32_CountFiles;
            }
         }
      }
      if (q_NewItemAtCurrentPosition == false)
      {
         ++sn_Counter;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns and select the application on the position

   \param[in]  orc_Pos  Position of application

   \return
   Pointer to application
   NULL if no application was at the position
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpUpdatePackageListNodeItemWidget * C_SyvUpUpdatePackageListNodeWidget::GetAndSelectApplication(
   const QPoint & orc_Pos) const
{
   C_SyvUpUpdatePackageListNodeItemWidget * const pc_App = this->GetApplication(orc_Pos);

   if (pc_App != NULL)
   {
      pc_App->SetSelected(true);
   }

   return pc_App;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the application on the position

   \param[in]   orc_Pos       Position of application
   \param[out]  opu32_Number  Optional parameter for found app number

   \return
   Pointer to application
   NULL if no application was at the position
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpUpdatePackageListNodeItemWidget * C_SyvUpUpdatePackageListNodeWidget::GetApplication(const QPoint & orc_Pos,
                                                                                            uint32 * const opu32_Number)
const
{
   C_SyvUpUpdatePackageListNodeItemWidget * pc_App = NULL;

   if (this->mpc_Ui->pc_PbExpColl->isChecked() == true)
   {
      sintn sn_Counter;
      const QPoint c_AdaptedPos = this->mpc_Ui->pc_WidgetAppList->mapFrom(this->parentWidget(), orc_Pos);

      // The header widget area shall be excluded, it can cause problems if a scrollbar is in the list available
      if (c_AdaptedPos.y() > 0)
      {
         for (sn_Counter = 0; sn_Counter < this->mpc_Ui->pc_AppVerticalLayout->count(); ++sn_Counter)
         {
            QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_Counter);

            if (pc_Item != NULL)
            {
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               pc_App = dynamic_cast<C_SyvUpUpdatePackageListNodeItemWidget *>(pc_Item->widget());

               if (pc_App != NULL)
               {
                  const QPoint c_AppPos = pc_App->mapFrom(this->parentWidget(), orc_Pos);

                  if (pc_App->rect().contains(c_AppPos) == true)
                  {
                     if (opu32_Number != NULL)
                     {
                        *opu32_Number = static_cast<uint32>(sn_Counter);
                     }
                     break;
                  }
               }
            }
         }
      }
   }

   return pc_App;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the select state of an specific application

   \param[in]  ou32_Application  Application number
   \param[in]  oq_Select         Flag if application should be selected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::SetApplicationSelect(const uint32 ou32_Application, const bool oq_Select) const
{
   QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(ou32_Application);

   // Return the path of the application
   if (pc_Item != NULL)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageListNodeItemWidget * const pc_App =
         dynamic_cast<C_SyvUpUpdatePackageListNodeItemWidget *>(pc_Item->widget());

      if (pc_App != NULL)
      {
         pc_App->SetSelected(oq_Select);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the number of files

   \return
   Number of files
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpUpdatePackageListNodeWidget::GetFileCount(void) const
{
   return this->mu32_FileCount;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns if at least one file has to be updated

   \return
   true     Update is necessary
   false    No update is necessary
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdatePackageListNodeWidget::IsUpdateNecessary(void) const
{
   sintn sn_Counter;
   bool q_ApplicationUpdateNecessary = false;

   for (sn_Counter = 0; sn_Counter < (this->mpc_Ui->pc_AppVerticalLayout->count() - 1); ++sn_Counter)
   {
      if (this->m_GetApplicationState(sn_Counter) != C_SyvUpUpdatePackageListNodeItemWidget::hu32_STATE_FINISHED)
      {
         q_ApplicationUpdateNecessary = true;
      }
   }
   return q_ApplicationUpdateNecessary;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates and returns the update package with all information for the system update for the concrete node

   \param[out]  orc_ApplicationsToWrite   Vector with node update configuration
   \param[out]  opc_AllApplications       Optional vector with all node applications
   \param[out]  oru32_FilesUpdated        Number of already updated files or no update necessary

   \return
   C_NO_ERR    Update package with all information created
   C_RD_WR     At least one file does not exist
   C_NOACT     No files for applications to write added
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvUpUpdatePackageListNodeWidget::GetUpdatePackage(C_OSCSuSequences::C_DoFlash & orc_ApplicationsToWrite,
                                                            C_OSCSuSequences::C_DoFlash * const opc_AllApplications,
                                                            stw_types::uint32 & oru32_FilesUpdated) const
{
   sint32 s32_Return = C_NO_ERR;
   sintn sn_Counter;

   for (sn_Counter = 0; sn_Counter < this->mpc_Ui->pc_AppVerticalLayout->count(); ++sn_Counter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_Counter);

      if (pc_Item != NULL)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SyvUpUpdatePackageListNodeItemWidget * const pc_App =
            dynamic_cast<C_SyvUpUpdatePackageListNodeItemWidget *>(pc_Item->widget());

         // Add only if application is not already finished. In this case an update of this application is not
         // necessary.
         if (pc_App != NULL)
         {
            const QString c_Path = pc_App->GetAppAbsoluteFilePath();
            QFileInfo c_File(c_Path);

            // Check file
            if ((c_File.exists() == true) &&
                (c_File.isFile() == true))
            {
               const QString c_LogEntry =
                  "Generate Update Package: For node \"%1\" and application \"%2\" used file: %3";

               if (pc_App->GetState() != C_SyvUpUpdatePackageListNodeItemWidget::hu32_STATE_FINISHED)
               {
                  orc_ApplicationsToWrite.c_FilesToFlash.push_back(stw_scl::C_SCLString(c_Path.toStdString().c_str()));
               }
               else
               {
                  // Update of application is not necessary
                  ++oru32_FilesUpdated;
               }

               if (opc_AllApplications != NULL)
               {
                  // Fill vector with all applications independent of the state
                  opc_AllApplications->c_FilesToFlash.push_back(stw_scl::C_SCLString(c_Path.toStdString().c_str()));
               }

               osc_write_log_info("Generate Update Package",
                                  C_GtGetText::h_GetText(c_LogEntry.arg(this->mc_NodeName,
                                                                        pc_App->GetAppName(),
                                                                        c_Path).toStdString().c_str()));
            }
            else
            {
               const QString c_LogEntry =
                  "Generate Update Package: The path of Node \"%1\" and application \"%2\" is invalid: %3";

               osc_write_log_info("Generate Update Package",
                                  C_GtGetText::h_GetText(c_LogEntry.arg(this->mc_NodeName,
                                                                        pc_App->GetAppName(),
                                                                        c_Path).toStdString().c_str()));

               s32_Return = C_RD_WR;
            }
         }
      }

      if (s32_Return != C_NO_ERR)
      {
         break;
      }
   }

   if (orc_ApplicationsToWrite.c_FilesToFlash.size() == 0)
   {
      // No files added
      s32_Return = C_NOACT;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if mime valid

   \param[in,out]  orc_PathList           File paths to analyze, relevant paths in opc_RelevantFilePaths in
                                          will be removed here
   \param[in]      orc_Pos                Mouse position
   \param[out]     opc_RelevantFilePaths  File paths which could be used for this list
   \param[out]     oppc_App               Found application widget

   \return
   true  Valid
   false Invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdatePackageListNodeWidget::CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos,
                                                   QStringList * const opc_RelevantFilePaths,
                                                   C_SyvUpUpdatePackageListNodeItemWidget ** const oppc_App) const
{
   // One application is only usable in case of one specific path
   if (orc_PathList.size() == 1)
   {
      uint32 u32_FoundAppNumber;
      uint32 u32_CounterApp;
      // Address based nodes can only adapt existing applications
      C_SyvUpUpdatePackageListNodeItemWidget * const pc_App = this->GetApplication(orc_Pos, &u32_FoundAppNumber);

      Q_UNUSED(orc_PathList)
      Q_UNUSED(opc_RelevantFilePaths)

      if (oppc_App != NULL)
      {
         *oppc_App = pc_App;
      }

      for (u32_CounterApp = 0U; u32_CounterApp < this->mu32_FileCount; ++u32_CounterApp)
      {
         if ((u32_CounterApp == u32_FoundAppNumber) &&
             (pc_App != NULL))
         {
            pc_App->SetSelected(true);
         }
         else
         {
            this->SetApplicationSelect(u32_CounterApp, false);
         }
      }
   }

   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Expand or collapse this list

   \param[in]  oq_Expand   true: expand; false: collapse
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::Expand(const bool oq_Expand)
{
   this->mpc_Ui->pc_PbExpColl->setChecked(oq_Expand);
   this->m_OnExpand(oq_Expand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get expand/collapse state.

   \retval   true    expanded
   \retval   false   collapsed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdatePackageListNodeWidget::IsExpanded(void) const
{
   return this->mpc_Ui->pc_WidgetAppList->isVisibleTo(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns path of specific application

   \param[in]  ou32_Application  Application number

   \return
   Path of application
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpUpdatePackageListNodeWidget::m_GetApplicationPath(const uint32 ou32_Application) const
{
   QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(ou32_Application);
   QString c_Path = "";

   // Return the path of the application
   if (pc_Item != NULL)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageListNodeItemWidget * const pc_App =
         dynamic_cast<C_SyvUpUpdatePackageListNodeItemWidget *>(pc_Item->widget());

      if (pc_App != NULL)
      {
         c_Path = pc_App->GetAppFilePath();
      }
   }

   return c_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapts the widget in case of a change of the file count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::m_FileCountChanged(void)
{
   this->resize(this->mpc_Ui->pc_WidgetAppList->width(),
                static_cast<sintn>((this->mu32_FileCount * 40U) + 32U));

   this->mpc_Ui->pc_LabCount->setText("(" + QString::number(this->mu32_FileCount) + ")");
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::m_SetApplicationState(const uint32 ou32_Application,
                                                               const uint32 ou32_State) const
{
   QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(ou32_Application);

   // Adapt the icon of the finished application
   if (pc_Item != NULL)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageListNodeItemWidget * const pc_App =
         dynamic_cast<C_SyvUpUpdatePackageListNodeItemWidget *>(pc_Item->widget());

      if (pc_App != NULL)
      {
         pc_App->SetState(ou32_State);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   Here: expand/collapse on  mouse press

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::mousePressEvent(QMouseEvent * const opc_Event)
{
   QWidget::mousePressEvent(opc_Event);

   if ((this->mpc_Ui->pc_ListTitleWidget->rect().contains(opc_Event->pos()) == true) &&
       (opc_Event->button() == Qt::LeftButton))
   {
      this->mpc_Ui->pc_PbExpColl->toggle();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of expand signal.

   \param[in]  oq_Expand   true: expand; false: collapse
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::m_OnExpand(const bool oq_Expand)
{
   this->mpc_Ui->pc_WidgetAdd->setVisible(
      oq_Expand && this->mq_ShowAddButton && (this->mu32_FileCount == 0U));
   this->mpc_Ui->pc_WidgetAppList->setVisible(oq_Expand);

   this->updateGeometry();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of the applications
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::m_InitItems(void)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      this->mc_DeviceType = pc_Node->c_DeviceType.c_str();

      tgl_assert(pc_Node->pc_DeviceDefinition != NULL);
      this->mq_FileBased = pc_Node->pc_DeviceDefinition->q_FlashloaderOpenSydeIsFileBased;
      this->mq_StwFlashloader = (pc_Node->c_Properties.e_FlashLoader == C_OSCNodeProperties::eFL_STW);

      if (pc_View != NULL)
      {
         const C_PuiSvNodeUpdate * const pc_UpdateInfo = pc_View->GetNodeUpdateInformation(this->mu32_NodeIndex);

         tgl_assert(pc_UpdateInfo != NULL);
         if (pc_UpdateInfo != NULL)
         {
            QSpacerItem * const pc_Spacer = new QSpacerItem(0, 3, QSizePolicy::Minimum, QSizePolicy::Expanding);

            this->m_InitSpecificItem(*pc_Node, *pc_UpdateInfo);

            this->mpc_Ui->pc_AppVerticalLayout->addSpacerItem(pc_Spacer);
            this->mpc_Ui->pc_AppVerticalLayout->setStretch(this->mpc_Ui->pc_AppVerticalLayout->count() - 1, 1);

            if ((this->mq_ShowAddButton == true) &&
                (this->mu32_FileCount == 0U) &&
                (this->mpc_Ui->pc_PbExpColl->isChecked() == true)) // Is expanded?
            {
               this->mpc_Ui->pc_WidgetAdd->setVisible(true);
            }
            //lint -e{429}  no memory leak because of the parent of pc_Spacer and the Qt memory management
         }
      }
   }

   this->m_FileCountChanged();
}

//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpUpdatePackageListNodeWidget::m_GetApplicationState(const uint32 ou32_Application) const
{
   QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(ou32_Application);
   // Undefined state in error case
   uint32 u32_Return = 0xFFFFFFFFUL;

   // Adapt the icon of the finished application
   if (pc_Item != NULL)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageListNodeItemWidget * const pc_App =
         dynamic_cast<C_SyvUpUpdatePackageListNodeItemWidget *>(pc_Item->widget());

      if (pc_App != NULL)
      {
         u32_Return = pc_App->GetState();
      }
   }

   return u32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::m_SetApplicationConnected(const uint32 ou32_Application,
                                                                   const bool oq_Connected) const
{
   QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(ou32_Application);

   // Return the path of the application
   if (pc_Item != NULL)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageListNodeItemWidget * const pc_App =
         dynamic_cast<C_SyvUpUpdatePackageListNodeItemWidget *>(pc_Item->widget());

      if (pc_App != NULL)
      {
         pc_App->SetConnected(oq_Connected);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpUpdatePackageListNodeWidget::m_GetFirstNotFinishedApplication(void) const
{
   sint32 s32_ApplicationCounter;

   for (s32_ApplicationCounter = 0U;
        s32_ApplicationCounter < static_cast<sint32>(this->mpc_Ui->pc_AppVerticalLayout->count() - 1);
        ++s32_ApplicationCounter)
   {
      if (this->m_GetApplicationState(s32_ApplicationCounter) !=
          C_SyvUpUpdatePackageListNodeItemWidget::hu32_STATE_FINISHED)
      {
         break;
      }
   }

   return static_cast<uint32>(s32_ApplicationCounter);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::m_UpdateNumbers(void) const
{
   sintn sn_AppWidgetCounter;

   for (sn_AppWidgetCounter = 0; sn_AppWidgetCounter < this->mpc_Ui->pc_AppVerticalLayout->count();
        ++sn_AppWidgetCounter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_AppWidgetCounter);

      if (pc_Item != NULL)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SyvUpUpdatePackageListNodeItemWidget * const pc_App =
            dynamic_cast<C_SyvUpUpdatePackageListNodeItemWidget *>(pc_Item->widget());

         if (pc_App != NULL)
         {
            pc_App->SetAppNumber(static_cast<uint32>(sn_AppWidgetCounter));
         }
      }
   }
}
