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

#include "C_SyvUpPackageSectionNodeWidget.h"
#include "ui_C_SyvUpPackageSectionNodeWidget.h"

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
#include "C_PuiUtil.h"
#include "C_SyvUpPackageListNodeItemParamSetWidget.h"
#include "C_SyvUpPackageListNodeItemPemFileWidget.h"
#include "C_SyvUpParamSetFileAddPopUp.h"

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
C_SyvUpPackageSectionNodeWidget::C_SyvUpPackageSectionNodeWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpPackageSectionNodeWidget),
   mu32_ViewIndex(0U),
   mu32_NodeIndex(0U),
   mu32_SectionNumber(0U),
   mc_NodeName(""),
   mc_DeviceType(""),
   mq_FileBased(false),
   mq_StwFlashloader(false),
   mu32_FileCount(0U),
   mu32_PrimaryFileCount(0U),
   mu32_ParamSetFileCount(0U),
   mu32_PemFileCount(0U),
   mu32_DataBlockPathNumber(0U),
   mq_ShowAddButton(false),
   mu32_PositionNumber(0U),
   //lint -e{1938}  static const is guaranteed preinitialized before main
   mu32_State(C_SyvUpPackageListNodeItemWidget::hu32_STATE_DEFAULT),
   mq_Connected(false),
   mpc_Movie(new QMovie("://images/system_views/UpdateAnimationInProgress.gif", QByteArray(), this))
{
   QSizePolicy c_SizePolicy;

   mpc_Ui->setupUi(this);

   this->mpc_Movie->setScaledSize(QSize(20, 20));

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
           &C_SyvUpPackageSectionNodeWidget::m_OnExpand);

   //retain icon labe size when hidden
   c_SizePolicy = this->mpc_Ui->pc_LabelStatusIcon->sizePolicy();
   c_SizePolicy.setRetainSizeWhenHidden(true);
   this->mpc_Ui->pc_LabelStatusIcon->setSizePolicy(c_SizePolicy);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_Movie and the Qt memory management
C_SyvUpPackageSectionNodeWidget::~C_SyvUpPackageSectionNodeWidget()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of the widget

   \param[in]  ou32_ViewIndex             View index
   \param[in]  ou32_PositionNumber        Position number of node for showing
   \param[in]  ou32_NodeIndex             Node index
   \param[in]  orc_NodeName               Name of the node
   \param[in]  ou32_SectionNumber         Number of node section in list
   \param[in]  ou32_DataBlockPathNumber   Number of Data Block output file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::InitWidget(const uint32 ou32_ViewIndex, const uint32 ou32_PositionNumber,
                                                 const uint32 ou32_NodeIndex, const QString & orc_NodeName,
                                                 const uint32 ou32_SectionNumber, const uint32 ou32_DataBlockPathNumber)
{
   this->mu32_ViewIndex = ou32_ViewIndex;
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mc_NodeName = orc_NodeName;
   this->mu32_PositionNumber = ou32_PositionNumber;
   this->mu32_SectionNumber = ou32_SectionNumber;
   this->mu32_DataBlockPathNumber = ou32_DataBlockPathNumber;

   this->InitStaticNames();

   // Load applications
   this->m_InitItems();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::InitStaticNames(void)
{
   this->mpc_Ui->pc_LabNoFiles->setText(C_GtGetText::h_GetText(
                                           "No files.\nAdd any via the context menu or drag and drop."));

   this->m_InitStaticNames();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget for the connected state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::SetConnected(void)
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
void C_SyvUpPackageSectionNodeWidget::SetUpdateApplicationStarted(void)
{
   this->m_SetFileState(this->m_GetFirstNotFinishedApplication(),
                        C_SyvUpPackageListNodeItemWidget::hu32_STATE_IN_PROGRESS);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the updated was started
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::SetUpdateApplicationFinished(void)
{
   this->m_SetFileState(
      this->m_GetFirstNotFinishedApplication(), C_SyvUpPackageListNodeItemWidget::hu32_STATE_FINISHED);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the application update hat an error
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::SetUpdateApplicationError(void)
{
   this->m_SetFileState(
      this->m_GetFirstNotFinishedApplication(), C_SyvUpPackageListNodeItemWidget::hu32_STATE_ERROR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the entire update was finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::SetUpdateFinished(void)
{
   if (this->mq_FileBased == true)
   {
      uint32 u32_Counter;

      // Special case: After updating a file based node, it is not possible to read back or confirm
      // the state of the node. Reset the state of all applications
      for (u32_Counter = 0U; u32_Counter < this->mu32_FileCount; ++u32_Counter)
      {
         this->m_SetFileState(u32_Counter, C_SyvUpPackageListNodeItemWidget::hu32_STATE_TO_DO);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger device application information discard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::DiscardApplicationStatus(void)
{
   for (uint32 u32_Counter = 0U; u32_Counter < this->mu32_FileCount; ++u32_Counter)
   {
      this->m_SetFileState(u32_Counter, C_SyvUpPackageListNodeItemWidget::hu32_STATE_TO_DO);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget for the disconnected state

   Reset the application states
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::SetDisconnected(void)
{
   uint32 u32_Counter;

   this->mq_Connected = false;

   if (this->mu32_FileCount > 0U)
   {
      for (u32_Counter = 0U; u32_Counter < this->mu32_FileCount; ++u32_Counter)
      {
         this->m_SetFileState(u32_Counter, C_SyvUpPackageListNodeItemWidget::hu32_STATE_DEFAULT);
         this->m_SetApplicationConnected(u32_Counter, false);
      }
   }
   else
   {
      this->m_SetState(C_SyvUpPackageListNodeItemWidget::hu32_STATE_DISABLED);
   }

   this->mpc_Ui->pc_WidgetAdd->setEnabled(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the position number

   \param[in]  ou32_PositionNumber  New position number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::UpdatePositionNumber(const uint32 ou32_PositionNumber)
{
   this->mu32_PositionNumber = ou32_PositionNumber;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new file for the application

   \param[in]  orc_File    New path
   \param[in]  opc_App     Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::AdaptFile(const QString & orc_File,
                                                C_SyvUpPackageListNodeItemWidget * const opc_App)
{
   if (opc_App != NULL)
   {
      // Adapt the widget itself
      opc_App->SetAppFile(orc_File, false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the skip update file flag

   \param[in]  oq_Skip  New skip flag
   \param[in]  opc_App  Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::SetSkipOfUpdateFile(const bool oq_Skip,
                                                          C_SyvUpPackageListNodeItemWidget * const opc_App)
{
   if (opc_App != NULL)
   {
      opc_App->SetSkipOfUpdateFile(oq_Skip);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Revert the current application to the system definition default path

   Default implementation can not revert files

   \param[in]  opc_App  Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_SyvUpPackageSectionNodeWidget::RevertFile(C_SyvUpPackageListNodeItemWidget * const opc_App)
{
   Q_UNUSED(opc_App)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes the current application

   \param[in]  opc_App  Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::RemoveFile(C_SyvUpPackageListNodeItemWidget * const opc_App)
{
   if (opc_App != NULL)
   {
      // Remove the widget
      this->mpc_Ui->pc_FileVerticalLayout->removeWidget(opc_App);
      opc_App->setParent(NULL);
      delete opc_App;

      this->m_UpdateNumbers();
      if (this->mu32_FileCount > 0)
      {
         --this->mu32_FileCount;
      }
      this->m_FileCountChanged();
   }

   this->mpc_Ui->pc_WidgetAdd->setVisible(this->mpc_Ui->pc_FileVerticalLayout->count() == 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes all application paths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::RemoveAllFiles(void)
{
   sintn sn_Counter;

   for (sn_Counter = this->mpc_Ui->pc_FileVerticalLayout->count() - 1; sn_Counter >= 0; --sn_Counter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(sn_Counter);

      if (pc_Item != NULL)
      {
         C_SyvUpPackageListNodeItemWidget * const pc_App =
            dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_Item->widget());

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
   Default implementation sets all files to hu32_STATE_TO_DO. If no file exist, the state will be set to disabled.

   \param[in]  orc_DeviceInformation   Device info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::UpdateDeviceInformation(const C_SyvUpDeviceInfo & orc_DeviceInformation)
{
   sintn sn_AppWidgetCounter;
   bool q_AtLeastOneFileToDo = false;

   Q_UNUSED(orc_DeviceInformation)

   //go through all applications in the UI
   for (sn_AppWidgetCounter = 0; sn_AppWidgetCounter < this->mpc_Ui->pc_FileVerticalLayout->count();
        ++sn_AppWidgetCounter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(sn_AppWidgetCounter);

      if (pc_Item != NULL)
      {
         C_SyvUpPackageListNodeItemWidget * const pc_App =
            dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_Item->widget());

         if (pc_App != NULL)
         {
            pc_App->SetState(C_SyvUpPackageListNodeItemWidget::hu32_STATE_TO_DO);
            q_AtLeastOneFileToDo = true;
         }
      }
   }

   if (q_AtLeastOneFileToDo == true)
   {
      this->m_SetState(C_SyvUpPackageListNodeItemWidget::hu32_STATE_TO_DO);
   }
   else
   {
      this->m_SetState(C_SyvUpPackageListNodeItemWidget::hu32_STATE_DISABLED);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Opens the settings for a PEM file

   Default implementation does nothing.

   \param[in]  opc_App  Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_SyvUpPackageSectionNodeWidget::OpenPemFileSettings(C_SyvUpPackageListNodeItemWidget * const opc_App)
{
   Q_UNUSED(opc_App)
   // Nothing to do here
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks all paths for existence

   \param[out]  oru32_CountFiles             Number of files
   \param[out]  opc_MissingFiles             App names of files with missing file
   \param[out]  opc_MissingParamSetFiles     Parameter set image files names with missing file
   \param[out]  opc_FlashwareWarningsApps    App names of apps with flashware warnings

   \return
   C_NO_ERR    All files are existent
   C_CONFIG    Minimum one file was not found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvUpPackageSectionNodeWidget::CheckAllFiles(stw_types::uint32 & oru32_CountFiles,
                                                      QStringList * const opc_MissingFiles,
                                                      QStringList * const opc_MissingParamSetFiles,
                                                      QStringList * const opc_FlashwareWarningsApps)
{
   sintn sn_Counter = 0;
   sint32 s32_Return = C_NO_ERR;

   while (sn_Counter < this->mpc_Ui->pc_FileVerticalLayout->count())
   {
      bool q_NewItemAtCurrentPosition = false;
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(sn_Counter);

      if (pc_Item != NULL)
      {
         C_SyvUpPackageListNodeItemWidget * const pc_App =
            dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_Item->widget());

         if (pc_App != NULL)
         {
            // Check the app path
            const sint32 s32_CheckPathResult = pc_App->CheckPath();
            // missing files
            if (s32_CheckPathResult == C_CONFIG)
            {
               if ((opc_MissingFiles != NULL) || (opc_MissingParamSetFiles != NULL))
               {
                  const QString c_New =
                     static_cast<QString>("#%1 - %2 - %3 - %4").
                     arg(this->mu32_PositionNumber + 1U).
                     arg(this->mc_NodeName).
                     arg(this->mc_SectionName).
                     arg(pc_App->GetAppFileName());

                  if ((opc_MissingFiles != NULL) &&
                      (pc_App->GetType() != mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PARAMSET))
                  {
                     opc_MissingFiles->push_back(c_New);
                  }
                  else if (opc_MissingParamSetFiles != NULL)
                  {
                     opc_MissingParamSetFiles->push_back(c_New);
                  }
                  else
                  {
                     // Nothing to do
                  }
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
                        static_cast<QString>(C_GtGetText::h_GetText(
                                                "#%1 - %2 - %3 - %4: Device type %5 does not match node type %6")).
                        arg(this->mu32_PositionNumber + 1U).
                        arg(this->mc_NodeName).arg(this->mc_SectionName).
                        arg(pc_App->GetAppFileName()).
                        arg(pc_App->GetAppDeviceType()).
                        arg(this->mc_DeviceType);
                  }
                  else
                  {
                     c_New =
                        static_cast<QString>(C_GtGetText::h_GetText(
                                                "#%1 - %2 - %3 - %4: HEX file has multiple application blocks.")).
                        arg(this->mu32_PositionNumber + 1U).
                        arg(this->mc_NodeName).
                        arg(this->mc_SectionName).
                        arg(pc_App->GetAppFileName());
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
C_SyvUpPackageListNodeItemWidget * C_SyvUpPackageSectionNodeWidget::GetAndSelectApplication(const QPoint & orc_Pos)
const
{
   C_SyvUpPackageListNodeItemWidget * const pc_App = this->GetApplication(orc_Pos);

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
C_SyvUpPackageListNodeItemWidget * C_SyvUpPackageSectionNodeWidget::GetApplication(const QPoint & orc_Pos,
                                                                                   uint32 * const opu32_Number)
const
{
   C_SyvUpPackageListNodeItemWidget * pc_Return = NULL;

   if (this->mpc_Ui->pc_PbExpColl->isChecked() == true)
   {
      sintn sn_Counter;
      const QPoint c_AdaptedPos = this->mpc_Ui->pc_WidgetFileList->mapFrom(this->parentWidget(), orc_Pos);

      // The header widget area shall be excluded, it can cause problems if a scrollbar is in the list available
      if (c_AdaptedPos.y() > 0)
      {
         for (sn_Counter = 0; sn_Counter < this->mpc_Ui->pc_FileVerticalLayout->count(); ++sn_Counter)
         {
            QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(sn_Counter);

            if (pc_Item != NULL)
            {
               C_SyvUpPackageListNodeItemWidget * const pc_App =
                  dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_Item->widget());

               if (pc_App != NULL)
               {
                  const QPoint c_AppPos = pc_App->mapFrom(this->parentWidget(), orc_Pos);

                  if (pc_App->rect().contains(c_AppPos) == true)
                  {
                     pc_Return = pc_App;

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

   return pc_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the select state of an specific application

   \param[in]  ou32_Application  Application number
   \param[in]  oq_Select         Flag if application should be selected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::SetApplicationSelect(const uint32 ou32_Application, const bool oq_Select) const
{
   QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(ou32_Application);

   // Return the path of the application
   if (pc_Item != NULL)
   {
      C_SyvUpPackageListNodeItemWidget * const pc_App =
         dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_Item->widget());

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
uint32 C_SyvUpPackageSectionNodeWidget::GetFileCount(void) const
{
   return this->mu32_FileCount;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the number of primary files of the section type without parameter set image files

   \return
   Number of files
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpPackageSectionNodeWidget::GetPrimaryFileCount(void) const
{
   return this->mu32_PrimaryFileCount;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the number of parameter image set files

   \return
   Number of files
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpPackageSectionNodeWidget::GetParamSetFileCount(void) const
{
   return this->mu32_ParamSetFileCount;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the number of parameter image set files

   \return
   Number of files
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpPackageSectionNodeWidget::GetPemFileCount(void) const
{
   return this->mu32_PemFileCount;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the state of the section

   \return
   State value
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpPackageSectionNodeWidget::GetSectionState(void) const
{
   uint32 u32_State = this->mu32_State;

   if (this->m_AreAllFilesSkipped() == true)
   {
      // Special case: Handle file as finished to skip the file
      u32_State = C_SyvUpPackageListNodeItemWidget::hu32_STATE_FINISHED;
   }

   return u32_State;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns if at least one file has to be updated

   \return
   true     Update is necessary
   false    No update is necessary
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPackageSectionNodeWidget::IsUpdateNecessary(void) const
{
   sintn sn_Counter;
   bool q_ApplicationUpdateNecessary = false;

   for (sn_Counter = 0; sn_Counter < (this->mpc_Ui->pc_FileVerticalLayout->count()); ++sn_Counter)
   {
      if (this->m_GetApplicationState(sn_Counter) != C_SyvUpPackageListNodeItemWidget::hu32_STATE_FINISHED)
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
sint32 C_SyvUpPackageSectionNodeWidget::GetUpdatePackage(C_OSCSuSequences::C_DoFlash & orc_ApplicationsToWrite,
                                                         C_OSCSuSequences::C_DoFlash * const opc_AllApplications,
                                                         stw_types::uint32 & oru32_FilesUpdated) const
{
   sint32 s32_Return = C_NO_ERR;
   sintn sn_Counter;

   for (sn_Counter = 0; sn_Counter < this->mpc_Ui->pc_FileVerticalLayout->count(); ++sn_Counter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(sn_Counter);

      if (pc_Item != NULL)
      {
         C_SyvUpPackageListNodeItemWidget * const pc_App =
            dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_Item->widget());

         // Add only if application is not already finished. In this case an update of this application is not
         // necessary.
         if (pc_App != NULL)
         {
            const QString c_Path = pc_App->GetAppAbsoluteFilePath();
            const bool q_ParamSetFile = (pc_App->GetType() == mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PARAMSET);
            const bool q_PemFile = (pc_App->GetType() == mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PEM);
            const QFileInfo c_File(c_Path);

            // Check file
            if ((c_File.exists() == true) &&
                (c_File.isFile() == true))
            {
               QString c_LogEntry;

               if ((q_ParamSetFile == false) &&
                   (q_PemFile == false))
               {
                  c_LogEntry = "Generate Update Package: For node \"%1\" and application \"%2\" used file: %3";
               }
               else if (q_ParamSetFile == true)
               {
                  c_LogEntry =
                     "Generate Update Package: For node \"%1\" and application \"%2\" used parameter set image file: %3";
               }
               else
               {
                  c_LogEntry = "Generate Update Package: For node \"%1\" and application \"%2\" used PEM file: %3";
               }

               if ((pc_App->GetSkipOfUpdateFile() == false) &&
                   (pc_App->GetState() != C_SyvUpPackageListNodeItemWidget::hu32_STATE_FINISHED))
               {
                  if ((q_ParamSetFile == false) &&
                      (q_PemFile == false))
                  {
                     orc_ApplicationsToWrite.c_FilesToFlash.push_back(c_Path.toStdString().c_str());
                  }
                  else if (q_ParamSetFile == true)
                  {
                     orc_ApplicationsToWrite.c_FilesToWriteToNvm.push_back(c_Path.toStdString().c_str());
                  }
                  else
                  {
                     orc_ApplicationsToWrite.c_PemFile = c_Path.toStdString().c_str();

                     mh_FillDoFlashWithPemStates(pc_App, orc_ApplicationsToWrite);
                  }
               }
               else
               {
                  // Update of application is not necessary
                  ++oru32_FilesUpdated;

                  if (q_PemFile == true)
                  {
                     // Special case, in case of a skipped PEM file, do not transfer the states
                     orc_ApplicationsToWrite.q_SendSecurityEnabledState = false;
                     orc_ApplicationsToWrite.q_SendDebuggerEnabledState = false;
                  }
               }

               if (opc_AllApplications != NULL)
               {
                  // Fill vector with all applications independent of the state
                  if ((q_ParamSetFile == false) &&
                      (q_PemFile == false))
                  {
                     opc_AllApplications->c_FilesToFlash.push_back(c_Path.toStdString().c_str());
                  }
                  else if (q_ParamSetFile == true)
                  {
                     opc_AllApplications->c_FilesToWriteToNvm.push_back(c_Path.toStdString().c_str());
                  }
                  else
                  {
                     opc_AllApplications->c_PemFile = c_Path.toStdString().c_str();

                     mh_FillDoFlashWithPemStates(pc_App, *opc_AllApplications);
                  }
               }

               osc_write_log_info("Generate Update Package",
                                  C_GtGetText::h_GetText(c_LogEntry.arg(this->mc_NodeName,
                                                                        this->mc_SectionName,
                                                                        c_Path).toStdString().c_str()));
            }
            else
            {
               QString c_LogEntry;

               if ((q_ParamSetFile == false) &&
                   (q_PemFile == false))
               {
                  c_LogEntry = "Generate Update Package: The path of Node \"%1\" and application \"%2\" is invalid: %3";
               }
               else if (q_ParamSetFile == true)
               {
                  c_LogEntry = "Generate Update Package: The path of Node \"%1\" and application \"%2\" parameter"
                               " set image file is invalid: %3";
               }
               else
               {
                  c_LogEntry = "Generate Update Package: The path of Node \"%1\" and application \"%2\" PEM"
                               " file is invalid: %3";
               }

               osc_write_log_info("Generate Update Package",
                                  C_GtGetText::h_GetText(c_LogEntry.arg(this->mc_NodeName,
                                                                        this->mc_SectionName,
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

   if ((orc_ApplicationsToWrite.c_FilesToFlash.size() == 0) &&
       (orc_ApplicationsToWrite.c_FilesToWriteToNvm.size() == 0) &&
       (orc_ApplicationsToWrite.c_PemFile == ""))
   {
      // No files added
      s32_Return = C_NOACT;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if mime valid

   \param[in,out]  orc_PathList                    File paths to analyze, relevant paths in opc_RelevantFilePaths in
                                                   will be removed here
   \param[in]      orc_Pos                         Mouse position
   \param[out]     opc_RelevantFilePaths           File paths which could be used for this list
   \param[out]     opc_RelevantParamSetImagePaths  Parameter set image paths which could be used for this list
   \param[out]     opc_RelevantPemFilePaths        PEM file paths which could be used for this list
   \param[out]     oppc_App                        Found application widget

   \return
   true  Valid
   false Invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPackageSectionNodeWidget::CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos,
                                                QStringList * const opc_RelevantFilePaths,
                                                QStringList * const opc_RelevantParamSetImagePaths,
                                                QStringList * const opc_RelevantPemFilePaths,
                                                C_SyvUpPackageListNodeItemWidget ** const oppc_App) const
{
   // One application is only usable in case of one specific path
   if (orc_PathList.size() == 1)
   {
      uint32 u32_FoundAppNumber;
      uint32 u32_CounterApp;
      // Address based nodes can only adapt existing applications
      C_SyvUpPackageListNodeItemWidget * const pc_App = this->GetApplication(orc_Pos, &u32_FoundAppNumber);

      Q_UNUSED(orc_PathList)
      Q_UNUSED(opc_RelevantFilePaths)
      Q_UNUSED(opc_RelevantParamSetImagePaths)
      Q_UNUSED(opc_RelevantPemFilePaths)

      if (oppc_App != NULL)
      {
         *oppc_App = pc_App;
      }

      for (u32_CounterApp = 0U; u32_CounterApp < this->mu32_FileCount; ++u32_CounterApp)
      {
         if ((pc_App != NULL) &&
             (u32_CounterApp == u32_FoundAppNumber))
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
void C_SyvUpPackageSectionNodeWidget::Expand(const bool oq_Expand)
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
bool C_SyvUpPackageSectionNodeWidget::IsExpanded(void) const
{
   return this->mpc_Ui->pc_WidgetFileList->isVisibleTo(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: Adapt the path text

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);

   // Update the label text for adapting the elided text if necessary
   this->m_UpdateTitle();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update of the title label
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::m_UpdateTitle(void)
{
   this->mpc_Ui->pc_LabTitle->setText(C_Uti::h_AdaptStringToSize(this->mc_Title,
                                                                 this->mpc_Ui->pc_LabTitle->fontMetrics(),
                                                                 this->width() - mhsn_TitleOffset));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the number of a param set file if necessary

   Default implementation does not change anything

   \param[in]  ou32_Number    Unedited number of param set file

   \return
   Adapted number of param set file
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpPackageSectionNodeWidget::m_AdaptParamSetNumber(const uint32 ou32_Number)
{
   return ou32_Number;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns path of specific application

   \param[in]  ou32_Application  Application number

   \return
   Path of application
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPackageSectionNodeWidget::m_GetApplicationPath(const uint32 ou32_Application) const
{
   QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(ou32_Application);
   QString c_Path = "";

   // Return the path of the application
   if (pc_Item != NULL)
   {
      C_SyvUpPackageListNodeItemWidget * const pc_App =
         dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_Item->widget());

      if (pc_App != NULL)
      {
         c_Path = pc_App->GetAppFilePath();
      }
   }

   return c_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if all files of the section are marked with skipped

   \retval   true    All files are marked with skipped
   \retval   false   At least one file is not marked with skipped or no files exist
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPackageSectionNodeWidget::m_AreAllFilesSkipped(void) const
{
   sintn sn_FileCounter;
   bool q_Return = false;
   bool q_AtLeastOneFileNotSkipped = false;
   bool q_AtLeastOneFileExist = false;

   for (sn_FileCounter = 0; sn_FileCounter < this->mpc_Ui->pc_FileVerticalLayout->count(); ++sn_FileCounter)
   {
      QLayoutItem * const pc_CurrentItem = this->mpc_Ui->pc_FileVerticalLayout->itemAt(sn_FileCounter);

      if (pc_CurrentItem != NULL)
      {
         C_SyvUpPackageListNodeItemWidget * const pc_App =
            dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_CurrentItem->widget());

         if (pc_App != NULL)
         {
            // One file exist at least
            q_AtLeastOneFileExist = true;
            if (pc_App->GetSkipOfUpdateFile() == false)
            {
               // Not skipped file
               q_AtLeastOneFileNotSkipped = true;
               break;
            }
         }
      }
   }

   if ((q_AtLeastOneFileExist == true) &&
       (q_AtLeastOneFileNotSkipped == false))
   {
      // All files of section are skipped
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapts the widget in case of a change of the file count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::m_FileCountChanged(void)
{
   const sintn sn_HeightFileList = static_cast<sintn>((this->mu32_FileCount * 23U));
   const sintn sn_Height = sn_HeightFileList + 31;

   this->resize(this->mpc_Ui->pc_WidgetFileList->width(), sn_Height);

   this->mpc_Ui->pc_LabCount->setText("(" + QString::number(this->mu32_FileCount) + ")");

   if (this->mu32_FileCount > 0U)
   {
      this->m_SetState(C_SyvUpPackageListNodeItemWidget::hu32_STATE_DEFAULT);
   }
   else
   {
      this->m_SetState(C_SyvUpPackageListNodeItemWidget::hu32_STATE_DISABLED);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::m_SetFileState(const uint32 ou32_File, const uint32 ou32_State)
{
   QLayoutItem * const pc_CurrentItem = this->mpc_Ui->pc_FileVerticalLayout->itemAt(ou32_File);

   // Adapt the icon of the finished application
   if (pc_CurrentItem != NULL)
   {
      C_SyvUpPackageListNodeItemWidget * const pc_App =
         dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_CurrentItem->widget());

      if (pc_App != NULL)
      {
         pc_App->SetState(ou32_State);
      }
   }

   // Special handling for state visualization in case of multiple files in this sections
   switch (ou32_State)
   {
   case C_SyvUpPackageListNodeItemWidget::hu32_STATE_FINISHED:
      {
         // In this case all other files must be set to finished already
         sintn sn_AppWidgetCounter;
         bool q_IsAtLeastOneFileNotFinished = false;

         for (sn_AppWidgetCounter = 0; sn_AppWidgetCounter < this->mpc_Ui->pc_FileVerticalLayout->count();
              ++sn_AppWidgetCounter)
         {
            QLayoutItem * const pc_WidgetItem = this->mpc_Ui->pc_FileVerticalLayout->itemAt(sn_AppWidgetCounter);

            if (pc_WidgetItem != NULL)
            {
               C_SyvUpPackageListNodeItemWidget * const pc_App =
                  dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_WidgetItem->widget());

               if ((pc_App != NULL) &&
                   (pc_App->GetState() != C_SyvUpPackageListNodeItemWidget::hu32_STATE_FINISHED))
               {
                  q_IsAtLeastOneFileNotFinished = true;
                  break;
               }
            }
         }

         if (q_IsAtLeastOneFileNotFinished == false)
         {
            this->m_SetState(ou32_State);
         }
      }
      break;
   case C_SyvUpPackageListNodeItemWidget::hu32_STATE_TO_DO:
      if ((this->mu32_FileCount == 0U) ||
          (this->m_AreAllFilesSkipped() == true)) // Special case: Check if all sub files are skipped
      {
         // In this case nothing to in this section
         this->m_SetState(C_SyvUpPackageListNodeItemWidget::hu32_STATE_FINISHED);
      }
      else
      {
         this->m_SetState(ou32_State);
      }
      break;
   case C_SyvUpPackageListNodeItemWidget::hu32_STATE_DEFAULT:     // This state affects all files
   case C_SyvUpPackageListNodeItemWidget::hu32_STATE_ERROR:       // This state affects all files
   case C_SyvUpPackageListNodeItemWidget::hu32_STATE_IN_PROGRESS: // This first file counts, so this can be set always
   default:                                                       // Should not happen
      this->m_SetState(ou32_State);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the state of the application

   The icon will be adapted

   \param[in]  ou32_State  State of application
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::m_SetState(const uint32 ou32_State)
{
   const QSize c_Size(20, 20);

   this->mu32_State = ou32_State;

   // Stop the GIF
   disconnect(this->mpc_Movie, &QMovie::finished, this, &C_SyvUpPackageSectionNodeWidget::m_RestartMovie);
   this->mpc_Movie->stop();

   this->mpc_Ui->pc_LabelStatusIcon->setVisible(ou32_State != C_SyvUpPackageListNodeItemWidget::hu32_STATE_DISABLED);

   switch (ou32_State)
   {
   case C_SyvUpPackageListNodeItemWidget::hu32_STATE_DEFAULT:
      this->mpc_Ui->pc_LabelStatusIcon->setPixmap(QIcon(":images/system_views/IconUpdateWaiting.svg").pixmap(c_Size));
      break;
   case C_SyvUpPackageListNodeItemWidget::hu32_STATE_ERROR:
      this->mpc_Ui->pc_LabelStatusIcon->setPixmap(QIcon(":images/system_views/IconUpdateError.svg").pixmap(c_Size));
      break;
   case C_SyvUpPackageListNodeItemWidget::hu32_STATE_FINISHED:
      this->mpc_Ui->pc_LabelStatusIcon->setPixmap(QIcon(":images/system_views/IconUpdateSuccess.svg").pixmap(c_Size));
      break;
   case C_SyvUpPackageListNodeItemWidget::hu32_STATE_IN_PROGRESS:
      // Start the GIF
      this->mpc_Ui->pc_LabelStatusIcon->setMovie(this->mpc_Movie);
      connect(this->mpc_Movie, &QMovie::finished, this, &C_SyvUpPackageSectionNodeWidget::m_RestartMovie);
      this->mpc_Movie->start();
      break;
   case C_SyvUpPackageListNodeItemWidget::hu32_STATE_TO_DO:
      this->mpc_Ui->pc_LabelStatusIcon->setPixmap(QIcon(":images/system_views/IconUpdateWaiting.svg").pixmap(c_Size));
      break;
   default:
      this->mpc_Ui->pc_LabelStatusIcon->setPixmap(QIcon(":images/system_views/IconUpdateWaiting.svg").pixmap(c_Size));
      break;
   }
   this->repaint();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the current path of a parameter set image file and checks its content

   \param[in]  orc_File    New path
   \param[in]  opc_App     Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::m_AdaptParamSetFile(const QString & orc_File,
                                                          C_SyvUpPackageListNodeItemWidget * const opc_App)
{
   C_PuiSvNodeUpdateParamInfo c_ParamFileInfo;

   if (this->m_GetParamsetFileInfo(orc_File, c_ParamFileInfo) == C_NO_ERR)
   {
      C_SyvUpPackageListNodeItemParamSetWidget * const pc_ParamSetWidget =
         dynamic_cast<C_SyvUpPackageListNodeItemParamSetWidget *>(opc_App);

      C_SyvUpPackageSectionNodeWidget::AdaptFile(orc_File, opc_App);

      tgl_assert(pc_ParamSetWidget != NULL);
      if (pc_ParamSetWidget != NULL)
      {
         pc_ParamSetWidget->SetParamInfo(c_ParamFileInfo);

         tgl_assert(C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformationParamInfo(
                       this->mu32_ViewIndex,
                       this->mu32_NodeIndex,
                       pc_ParamSetWidget->GetAppNumber(),
                       c_ParamFileInfo) == C_NO_ERR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get paramset file info

   \param[in]   orc_File            New path
   \param[out]  orc_ParamFileInfo   Read parameter set file information

   \return
   C_NO_ERR    File read
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvUpPackageSectionNodeWidget::m_GetParamsetFileInfo(const QString & orc_File,
                                                              C_PuiSvNodeUpdateParamInfo & orc_ParamFileInfo)
{
   sint32 s32_Return = C_NOACT;

   bool q_NewFile = true;
   uint32 u32_Counter;

   // Check if file was already added
   for (u32_Counter = 0U; u32_Counter < this->mu32_FileCount; ++u32_Counter)
   {
      if (C_PuiUtil::h_GetAbsolutePathFromProject(orc_File) ==
          C_PuiUtil::h_GetAbsolutePathFromProject(this->m_GetApplicationPath(u32_Counter)))
      {
         q_NewFile = false;
         break;
      }
   }

   // Add the file
   if (q_NewFile == true)
   {
      QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(this, this);
      C_SyvUpParamSetFileAddPopUp * const pc_InfoDialog =
         new C_SyvUpParamSetFileAddPopUp(*c_New, C_PuiUtil::h_GetAbsolutePathFromProject(orc_File), orc_File,
                                         this->mu32_NodeIndex);

      //Read file info
      const sint32 s32_ReadFileResult = pc_InfoDialog->ReadFile();
      if (s32_ReadFileResult == C_NO_ERR)
      {
         //Resize
         c_New->SetSize(QSize(1000, 761));

         if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
         {
            orc_ParamFileInfo = pc_InfoDialog->GetParamInfo();
            s32_Return = C_NO_ERR;
         }
      }
      else
      {
         QString c_Details = static_cast<QString>(C_GtGetText::h_GetText("File path: %1")).
                             arg(C_PuiUtil::h_GetAbsolutePathFromProject(orc_File)) + "\nReason: ";
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         switch (s32_ReadFileResult)
         {
         case C_RD_WR:
            c_Details += C_GtGetText::h_GetText(
               "File does not exist or has invalid structure. See log file for details.");
            break;
         case C_CHECKSUM:
            c_Details += C_GtGetText::h_GetText("File CRC is not correct.");
            break;
         case C_CONFIG:
            c_Details += C_GtGetText::h_GetText("File has missing content. See log file for details.");
            break;
         default:
            c_Details += static_cast<QString>(C_GtGetText::h_GetText("Unknown reason. Error code: %1")).arg(
               s32_ReadFileResult);
            break;
         }
         c_Message.SetHeading(C_GtGetText::h_GetText("Update package configuration"));
         c_Message.SetDescription(C_GtGetText::h_GetText("File is not a valid parameter set image file."));
         c_Message.SetDetails(c_Details);
         c_Message.SetCustomMinHeight(180, 250);
         c_Message.Execute();

         s32_Return = C_RD_WR;
      }

      if (c_New != NULL)
      {
         c_New->HideOverlay();
         c_New->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_InfoDialog and the Qt memory management
   else
   {
      // inform user that file already exists
      C_OgeWiCustomMessage c_Message(this);
      c_Message.SetHeading(C_GtGetText::h_GetText("Add file"));
      c_Message.SetDescription(
         static_cast<QString>(C_GtGetText::h_GetText(
                                 "The file is already contained in the Update Package for this node "
                                 "and therefore not added again.")));
      c_Message.SetDetails(
         static_cast<QString>(C_GtGetText::h_GetText("%1"))
         .arg(C_PuiUtil::h_GetAbsolutePathFromProject(orc_File)));
      c_Message.Execute();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   Here: expand/collapse on  mouse press

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::mousePressEvent(QMouseEvent * const opc_Event)
{
   QWidget::mousePressEvent(opc_Event);

   if ((this->mpc_Ui->pc_ListTitleWidget->rect().contains(opc_Event->pos()) == true) &&
       (opc_Event->button() == Qt::LeftButton))
   {
      this->mpc_Ui->pc_PbExpColl->toggle();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the file is an parameter set image file

   The check decides based on the file extension.

   \param[in]  orc_File    File to check

   \retval   true    The file is a parameter set file
   \retval   false   The file is not a parameter set file
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPackageSectionNodeWidget::mh_IsFileParamSetFile(const QString & orc_File)
{
   const QFileInfo c_File(orc_File);
   bool q_ParamSet = false;
   const QString c_FileExtension = "syde_psi";

   if (c_File.completeSuffix().toLower() == c_FileExtension)
   {
      q_ParamSet = true;
   }

   return q_ParamSet;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the file is an PEM file

   The check decides based on the file extension.

   \param[in]  orc_File    File to check

   \retval   true    The file is a PEM file
   \retval   false   The file is not a PEM file
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPackageSectionNodeWidget::mh_IsFilePemFile(const QString & orc_File)
{
   const QFileInfo c_File(orc_File);
   bool q_PemFile = false;
   const QString c_FileExtension = "pem";

   if (c_File.completeSuffix().toLower() == c_FileExtension)
   {
      q_PemFile = true;
   }

   return q_PemFile;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of expand signal.

   \param[in]  oq_Expand   true: expand; false: collapse
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::m_OnExpand(const bool oq_Expand)
{
   this->mpc_Ui->pc_WidgetAdd->setVisible(
      oq_Expand && this->mq_ShowAddButton && (this->mu32_FileCount == 0U));
   this->mpc_Ui->pc_WidgetFileList->setVisible(oq_Expand);

   this->updateGeometry();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of the applications
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::m_InitItems(void)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      this->mc_DeviceType = pc_Node->c_DeviceType.c_str();

      tgl_assert(pc_Node->pc_DeviceDefinition != NULL);
      tgl_assert(pc_Node->u32_SubDeviceIndex < pc_Node->pc_DeviceDefinition->c_SubDevices.size());
      this->mq_FileBased =
         pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].q_FlashloaderOpenSydeIsFileBased;
      this->mq_StwFlashloader = (pc_Node->c_Properties.e_FlashLoader == C_OSCNodeProperties::eFL_STW);

      if (pc_View != NULL)
      {
         const C_PuiSvNodeUpdate * const pc_UpdateInfo = pc_View->GetNodeUpdateInformation(this->mu32_NodeIndex);

         tgl_assert(pc_UpdateInfo != NULL);
         if (pc_UpdateInfo != NULL)
         {
            this->m_InitSpecificItem(*pc_Node, *pc_UpdateInfo);

            if ((this->mq_ShowAddButton == true) &&
                (this->mu32_FileCount == 0U) &&
                (this->mpc_Ui->pc_PbExpColl->isChecked() == true)) // Is expanded?
            {
               this->mpc_Ui->pc_WidgetAdd->setVisible(true);
            }
         } //lint !e429  //no memory leak because of the parent of pc_Spacer and the Qt memory management
      }
   }

   this->m_FileCountChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the state of an application and optional the type of the application

   \param[in]       ou32_Application    Index of application in node section
   \param[out]      opu32_Type          Optional: Type of found application

   \return
   Status of application
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpPackageSectionNodeWidget::m_GetApplicationState(const uint32 ou32_Application,
                                                              uint32 * const opu32_Type) const
{
   QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(ou32_Application);
   // Undefined state in error case
   uint32 u32_Return = 0xFFFFFFFFUL;

   // Adapt the icon of the finished application
   if (pc_Item != NULL)
   {
      C_SyvUpPackageListNodeItemWidget * const pc_App =
         dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_Item->widget());

      if (pc_App != NULL)
      {
         u32_Return = pc_App->GetState();
         if (opu32_Type != NULL)
         {
            *opu32_Type = pc_App->GetType();
         }
      }
   }

   return u32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::m_SetApplicationConnected(const uint32 ou32_Application,
                                                                const bool oq_Connected) const
{
   QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(ou32_Application);

   // Return the path of the application
   if (pc_Item != NULL)
   {
      C_SyvUpPackageListNodeItemWidget * const pc_App =
         dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_Item->widget());

      if (pc_App != NULL)
      {
         pc_App->SetConnected(oq_Connected);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpPackageSectionNodeWidget::m_GetFirstNotFinishedApplication(void) const
{
   sint32 s32_ApplicationCounter;
   uint32 u32_TypeCounter;

   // The order of application type is relevant and different to the shown order
   // Transfer order:
   // 1: Datablock applications
   // 2: File based other files
   // 3: PSI files
   // 4: PEM file

   for (u32_TypeCounter = 0U; u32_TypeCounter <= mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PEM; ++u32_TypeCounter)
   {
      uint32 u32_ExpectedType;
      bool q_Found = false;

      // Order of type numbers does not match the expected types, so a little remap is necessary
      switch (u32_TypeCounter)
      {
      case 0U:
         u32_ExpectedType = mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK;
         break;
      case 1U:
         u32_ExpectedType = mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_FILE;
         break;
      case 2U:
         u32_ExpectedType = mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PARAMSET;
         break;
      case 3U:
         u32_ExpectedType = mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PEM;
         break;
      default:
         break;
      }

      // Search all applications
      for (s32_ApplicationCounter = 0U;
           s32_ApplicationCounter < static_cast<sint32>(this->mpc_Ui->pc_FileVerticalLayout->count());
           ++s32_ApplicationCounter)
      {
         // Check state and type to get the correct order
         uint32 u32_CurrentType = mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK;
         const uint32 u32_State = this->m_GetApplicationState(s32_ApplicationCounter, &u32_CurrentType);
         if ((u32_State != C_SyvUpPackageListNodeItemWidget::hu32_STATE_FINISHED) &&
             (u32_CurrentType == u32_ExpectedType))
         {
            q_Found = true;
            break;
         }
      }

      if (q_Found == true)
      {
         break;
      }
   }

   return static_cast<uint32>(s32_ApplicationCounter);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::m_UpdateNumbers(void)
{
   sintn sn_AppWidgetCounter;

   // Recount the different types of files
   this->mu32_PrimaryFileCount = 0U;
   this->mu32_ParamSetFileCount = 0U;
   this->mu32_PemFileCount = 0U;

   for (sn_AppWidgetCounter = 0; sn_AppWidgetCounter < this->mpc_Ui->pc_FileVerticalLayout->count();
        ++sn_AppWidgetCounter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(sn_AppWidgetCounter);

      if (pc_Item != NULL)
      {
         C_SyvUpPackageListNodeItemWidget * const pc_App =
            dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_Item->widget());

         if (pc_App != NULL)
         {
            if ((pc_App->GetType() != mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PARAMSET) &&
                (pc_App->GetType() != mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PEM))
            {
               pc_App->SetAppNumber(this->mu32_PrimaryFileCount);
               ++this->mu32_PrimaryFileCount;
            }
            else if (pc_App->GetType() == mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PEM)
            {
               pc_App->SetAppNumber(this->mu32_PemFileCount);
               ++mu32_PemFileCount;
            }
            else
            {
               // The param set file number can be affected in case of a NVM HALC datablock
               pc_App->SetAppNumber(this->m_AdaptParamSetNumber(this->mu32_ParamSetFileCount));
               ++this->mu32_ParamSetFileCount;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Restart animation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::m_RestartMovie(void)
{
   this->mpc_Movie->start();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fills the PEM states in the DoFlash class

   \param[in]       opc_App       PEM file app
   \param[in,out]   orc_DoFlash   Detailed output parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeWidget::mh_FillDoFlashWithPemStates(
   const C_SyvUpPackageListNodeItemWidget * const opc_App, C_OSCSuSequences::C_DoFlash & orc_DoFlash)
{
   const C_SyvUpPackageListNodeItemPemFileWidget * const pc_PemApp =
      dynamic_cast<const C_SyvUpPackageListNodeItemPemFileWidget *>(opc_App);

   tgl_assert(pc_PemApp != NULL);
   if (pc_PemApp != NULL)
   {
      C_PuiSvNodeUpdate::E_StateSecurity e_StateSecurity;
      C_PuiSvNodeUpdate::E_StateDebugger e_StateDebugger;

      pc_PemApp->GetPemStates(e_StateSecurity, e_StateDebugger);

      switch (e_StateSecurity)
      {
      case C_PuiSvNodeUpdate::eST_SEC_NO_CHANGE:
         orc_DoFlash.q_SendSecurityEnabledState = false;
         break;
      case C_PuiSvNodeUpdate::eST_SEC_ACTIVATE:
         orc_DoFlash.q_SendSecurityEnabledState = true;
         orc_DoFlash.q_SecurityEnabled = true;
         break;
      case C_PuiSvNodeUpdate::eST_SEC_DEACTIVATE:
         orc_DoFlash.q_SendSecurityEnabledState = true;
         orc_DoFlash.q_SecurityEnabled = false;
         break;
      default:
         break;
      }

      switch (e_StateDebugger)
      {
      case C_PuiSvNodeUpdate::eST_DEB_NO_CHANGE:
         orc_DoFlash.q_SendDebuggerEnabledState = false;
         break;
      case C_PuiSvNodeUpdate::eST_DEB_ACTIVATE:
         orc_DoFlash.q_SendDebuggerEnabledState = true;
         orc_DoFlash.q_DebuggerEnabled = true;
         break;
      case C_PuiSvNodeUpdate::eST_DEB_DEACTIVATE:
         orc_DoFlash.q_SendDebuggerEnabledState = true;
         orc_DoFlash.q_DebuggerEnabled = false;
         break;
      default:
         break;
      }
   }
}
