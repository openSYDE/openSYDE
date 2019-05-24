//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for one node in the update package

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QScrollBar>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QFileInfo>

#include "C_SyvUpUpdatePackageNodeWidget.h"
#include "ui_C_SyvUpUpdatePackageNodeWidget.h"

#include "stwerrors.h"

#include "constants.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_OgeWiUtil.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_PuiProject.h"
#include "C_ImpUtil.h"
#include "C_OgeWiCustomMessage.h"
#include "C_UsHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_types::sint32 C_SyvUpUpdatePackageNodeWidget::mhs32_LAYOUT_THRESHOLD = 93; // Keep synced with update
// summary widget

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_ViewIndex       View index
   \param[in]     ou32_PositionNumber  Position number of node for showing
   \param[in]     ou32_NodeIndex       Node index
   \param[in]     orc_NodeName         Name of the node
   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpUpdatePackageNodeWidget::C_SyvUpUpdatePackageNodeWidget(const uint32 ou32_ViewIndex,
                                                               const uint32 ou32_PositionNumber,
                                                               const uint32 ou32_NodeIndex,
                                                               const QString & orc_NodeName,
                                                               QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpUpdatePackageNodeWidget),
   mu32_ViewIndex(ou32_ViewIndex),
   mu32_NodeIndex(ou32_NodeIndex),
   mc_NodeName(orc_NodeName),
   mq_FileBased(false),
   mq_StwFlashloader(false),
   mq_Connected(false),
   mq_EmptyOptionalSectionsVisible(true),
   mu32_PositionNumber(ou32_PositionNumber),
   mu32_FilesUpdated(0U)
{
   this->mpc_Ui->setupUi(this);

   stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameSepTop,
                                                                  "HasColor9Background", true);
   stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameSepMiddle,
                                                                  "HasColor9Background", true);
   stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameSepBottom,
                                                                  "HasColor9Background", true);

   this->mpc_Ui->pc_ScrollAreaWidget->SetBackgroundColor(0);

   this->m_UpdateTitle();

   this->m_Init();

   this->mpc_Ui->pc_WidgetTitle->SetColorReserved(mc_STYLE_GUIDE_COLOR_10);
   this->mpc_Ui->pc_WidgetTitle->SetColorTooMuch(mc_STYLE_GUIDE_COLOR_10);
   this->mpc_Ui->pc_WidgetTitle->SetColorFree(mc_STYLE_GUIDE_COLOR_0);

   this->mpc_Ui->pc_LabIconWarning->SetSvg("://images/system_views/IconWarning.svg");
   this->mpc_Ui->pc_LabIconWarning->SetToolTipInformation(
      C_GtGetText::h_GetText("Missing Files"),
      C_GtGetText::h_GetText("The Update Package for this node contains files that were not found."));
   this->mpc_Ui->pc_LabIconWarning->setVisible(false);

   this->mpc_Ui->pc_LabIconLock->SetSvg("://images/system_views/IconLockedGrey.svg");
   this->mpc_Ui->pc_LabIconLock->SetToolTipInformation(
      C_GtGetText::h_GetText("Update Package Locked"),
      C_GtGetText::h_GetText("Update Package cannot be configured while update mode is active."));
   this->mpc_Ui->pc_LabIconLock->setVisible(false);

   // widget "no files"
   this->mpc_Ui->pc_LabNoFiles->SetFontPixel(13, false, false);
   this->mpc_Ui->pc_LabNoFiles->SetForegroundColor(8);
   this->mpc_Ui->pc_LabNoFiles->setText(C_GtGetText::h_GetText(
                                           "No files.\nAdd any via the context menu or drag and drop."));
   this->mpc_Ui->pc_WidgetAdd->setVisible(false);

   //Drag & drop of files
   this->setAcceptDrops(true);

   // restore sections expand/collapse from user settings
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   if (pc_View != NULL)
   {
      bool q_Expand;

      q_Expand = C_UsHandler::h_GetInstance()->GetProjSvSetupView(pc_View->GetName()).
                 GetSvNode(orc_NodeName).GetSectionsExpanded()[mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK];

      this->mpc_Ui->pc_DatablockWidget->Expand(q_Expand && (this->mpc_Ui->pc_DatablockWidget->GetFileCount() != 0));

      q_Expand = C_UsHandler::h_GetInstance()->GetProjSvSetupView(pc_View->GetName()).
                 GetSvNode(orc_NodeName).GetSectionsExpanded()[mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PARAMSET];
      this->mpc_Ui->pc_ParamsetWidget->Expand(q_Expand && (this->mpc_Ui->pc_ParamsetWidget->GetFileCount() != 0));

      q_Expand = C_UsHandler::h_GetInstance()->GetProjSvSetupView(pc_View->GetName()).
                 GetSvNode(orc_NodeName).GetSectionsExpanded()[mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_FILE];
      this->mpc_Ui->pc_FileBasedWidget->Expand(q_Expand && (this->mpc_Ui->pc_FileBasedWidget->GetFileCount() != 0));
   }

   // Start defaults to make delegate work properly ("drag-mode")
   this->mpc_Ui->pc_ScrollArea->setVisible(false);
   this->mpc_Ui->pc_WidgetTitle->setMinimumHeight(30);
   this->mpc_Ui->pc_FrameSepTop->setVisible(false);
   this->mpc_Ui->pc_FrameSepMiddle->setVisible(false);
   this->mpc_Ui->pc_FrameSepBottom->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpUpdatePackageNodeWidget::~C_SyvUpUpdatePackageNodeWidget()
{
   // save user settings
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      QMap<uint32, bool> c_Flags;
      c_Flags[mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK] = this->mpc_Ui->pc_DatablockWidget->IsExpanded();
      c_Flags[mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PARAMSET] = this->mpc_Ui->pc_ParamsetWidget->IsExpanded();
      c_Flags[mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_FILE] = this->mpc_Ui->pc_FileBasedWidget->IsExpanded();
      C_UsHandler::h_GetInstance()->SetProjSvUpdateSectionsExpandedFlags(pc_View->GetName(), this->mc_NodeName,
                                                                         c_Flags);
   }

   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::InitStaticNames(void) const
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget for the connected state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::SetConnected(void)
{
   this->mq_Connected = true;

   this->mpc_Ui->pc_LabIconLock->setVisible(true);
   this->mpc_Ui->pc_WidgetAdd->setEnabled(false);

   // Reset progress bar
   this->mpc_Ui->pc_WidgetTitle->SetProgress(0U);

   this->mpc_Ui->pc_DatablockWidget->SetConnected();
   this->mpc_Ui->pc_ParamsetWidget->SetConnected();

   if (this->mq_FileBased == true)
   {
      this->mpc_Ui->pc_FileBasedWidget->SetConnected();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the updated was started
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::SetUpdateStarted(void) const
{
   bool q_ApplicationUpdateNecessary;

   q_ApplicationUpdateNecessary = this->mpc_Ui->pc_DatablockWidget->IsUpdateNecessary();
   q_ApplicationUpdateNecessary = this->mpc_Ui->pc_ParamsetWidget->IsUpdateNecessary() || q_ApplicationUpdateNecessary;

   if (this->mq_FileBased == true)
   {
      q_ApplicationUpdateNecessary = this->mpc_Ui->pc_FileBasedWidget->IsUpdateNecessary() ||
                                     q_ApplicationUpdateNecessary;
   }

   if (q_ApplicationUpdateNecessary == true)
   {
      // Reset progress bar
      this->mpc_Ui->pc_WidgetTitle->SetProgress(0U);
   }
   else
   {
      // No update necessary.
      this->mpc_Ui->pc_WidgetTitle->SetProgress(100U);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the application update was started
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::SetUpdateApplicationStarted(void) const
{
   C_SyvUpUpdatePackageListNodeWidget * const pc_List = this->m_GetNextListInUpdateOrder();

   tgl_assert(pc_List != NULL);
   if (pc_List != NULL)
   {
      pc_List->SetUpdateApplicationStarted();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the updated was started
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::SetUpdateApplicationFinished(void)
{
   const uint32 u32_FileCount = this->m_GetFileCount();
   // Call before increase
   C_SyvUpUpdatePackageListNodeWidget * const pc_List = this->m_GetNextListInUpdateOrder();

   ++this->mu32_FilesUpdated;

   if (u32_FileCount > 0)
   {
      this->mpc_Ui->pc_WidgetTitle->SetProgress((this->mu32_FilesUpdated * 100U) / u32_FileCount);
   }
   else
   {
      this->mpc_Ui->pc_WidgetTitle->SetProgress(100U);
   }

   tgl_assert(pc_List != NULL);
   if (pc_List != NULL)
   {
      pc_List->SetUpdateApplicationFinished();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the application update hat an error

   \param[in]     ou32_NodeIndex         Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::SetUpdateApplicationError(void) const
{
   C_SyvUpUpdatePackageListNodeWidget * const pc_List = this->m_GetNextListInUpdateOrder();

   tgl_assert(pc_List != NULL);
   if (pc_List != NULL)
   {
      pc_List->SetUpdateApplicationError();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the entire update was finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::SetUpdateFinished(void) const
{
   this->mpc_Ui->pc_DatablockWidget->SetUpdateFinished();
   this->mpc_Ui->pc_ParamsetWidget->SetUpdateFinished();

   if (this->mq_FileBased == true)
   {
      this->mpc_Ui->pc_FileBasedWidget->SetUpdateFinished();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger device application information discard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::DiscardApplicationStatus(void) const
{
   this->mpc_Ui->pc_DatablockWidget->DiscardApplicationStatus();
   this->mpc_Ui->pc_ParamsetWidget->DiscardApplicationStatus();

   if (this->mq_FileBased == true)
   {
      this->mpc_Ui->pc_FileBasedWidget->DiscardApplicationStatus();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget for the disconnected state

   Reset the application states
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::SetDisconnected(void)
{
   this->mu32_FilesUpdated = 0U;
   this->SetProgress(0U);
   this->mq_Connected = false;

   this->mpc_Ui->pc_LabIconLock->setVisible(false);
   this->mpc_Ui->pc_WidgetAdd->setEnabled(true);

   this->mpc_Ui->pc_DatablockWidget->SetDisconnected();
   this->mpc_Ui->pc_ParamsetWidget->SetDisconnected();

   if (this->mq_FileBased == true)
   {
      this->mpc_Ui->pc_FileBasedWidget->SetDisconnected();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update device information

   \param[in] orc_DeviceInformation Device info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::UpdateDeviceInformation(const C_SyvUpDeviceInfo & orc_DeviceInformation) const
{
   this->mpc_Ui->pc_DatablockWidget->UpdateDeviceInformation(orc_DeviceInformation);
   if (this->mq_StwFlashloader == false)
   {
      this->mpc_Ui->pc_ParamsetWidget->UpdateDeviceInformation(orc_DeviceInformation);
   }
   if (this->mq_FileBased == true)
   {
      this->mpc_Ui->pc_FileBasedWidget->UpdateDeviceInformation(orc_DeviceInformation);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the current progress for showing for all applications

   \param[in]     ou32_Percentage   Percentage of progress
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::SetProgress(const uint32 ou32_Percentage) const
{
   const uint32 u32_FileCount = this->m_GetFileCount();

   if (u32_FileCount > 0U)
   {
      const uint32 u32_FinishedNodesPercentage = (this->mu32_FilesUpdated * 100U) / u32_FileCount;
      this->mpc_Ui->pc_WidgetTitle->SetProgress(u32_FinishedNodesPercentage + (ou32_Percentage / u32_FileCount));
   }
   else
   {
      this->mpc_Ui->pc_WidgetTitle->SetProgress(0U);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the position number

   \param[in]     ou32_PositionNumber         New position number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::UpdatePositionNumber(const uint32 ou32_PositionNumber)
{
   this->mu32_PositionNumber = ou32_PositionNumber;
   this->m_UpdateTitle();

   this->mpc_Ui->pc_DatablockWidget->UpdatePositionNumber(this->mu32_PositionNumber);
   this->mpc_Ui->pc_ParamsetWidget->UpdatePositionNumber(this->mu32_PositionNumber);
   this->mpc_Ui->pc_FileBasedWidget->UpdatePositionNumber(this->mu32_PositionNumber);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new file for the file based file list or a parameter set file

   \param[in]     orc_File      New path
   \param[in]     oq_Paramset   Flag if file for update or paramset file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::AddNewFile(const QString & orc_File, const bool oq_Paramset)
{
   if (oq_Paramset == false)
   {
      // compare all data block and file based files
      if (this->m_CheckFileAlreadyContained(orc_File) == false)
      {
         this->mpc_Ui->pc_FileBasedWidget->AddFile(orc_File);
         this->mpc_Ui->pc_FileBasedWidget->Expand(true);
      }
   }
   else
   {
      // check if file already is contained is done in AddFile
      this->mpc_Ui->pc_ParamsetWidget->AddFile(orc_File);
      this->mpc_Ui->pc_ParamsetWidget->Expand(true);
   }

   // update visibility of optional sections
   this->UpdateSectionsVisibility();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new file for the application

   \param[in]     orc_File      New path
   \param[in]     opc_App       Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::AdaptFile(const QString & orc_File,
                                               C_SyvUpUpdatePackageListNodeItemWidget * const opc_App)
{
   C_SyvUpUpdatePackageListNodeWidget * const pc_Parent = m_GetAppParentList(opc_App);

   if (pc_Parent != NULL)
   {
      if (this->mq_FileBased == true)
      {
         // for file based nodes it is not allowed two add a file twice, independent of section where it gets added
         if (this->m_CheckFileAlreadyContained(orc_File) == false)
         {
            pc_Parent->AdaptFile(orc_File, opc_App);
         }
      }
      else
      {
         pc_Parent->AdaptFile(orc_File, opc_App);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Revert the current application to the system definition default path

   Only for address based devices

   \param[in]     opc_App       Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::RevertFile(C_SyvUpUpdatePackageListNodeItemWidget * const opc_App) const
{
   C_SyvUpUpdatePackageListNodeWidget * const pc_Parent = m_GetAppParentList(opc_App);

   if (pc_Parent != NULL)
   {
      pc_Parent->RevertFile(opc_App);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes the current application

   Only for file based devices

   \param[in]     opc_App       Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::RemoveFile(C_SyvUpUpdatePackageListNodeItemWidget * const opc_App) const
{
   C_SyvUpUpdatePackageListNodeWidget * const pc_Parent = m_GetAppParentList(opc_App);

   if (pc_Parent != NULL)
   {
      pc_Parent->RemoveFile(opc_App);

      // update visibility of optional sections
      this->UpdateSectionsVisibility();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes all application paths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::RemoveAllFiles(void) const
{
   this->mpc_Ui->pc_DatablockWidget->RemoveAllFiles();

   if (this->mq_StwFlashloader == false)
   {
      this->mpc_Ui->pc_ParamsetWidget->RemoveAllFiles();
   }

   if (this->mq_FileBased == true)
   {
      this->mpc_Ui->pc_FileBasedWidget->RemoveAllFiles();
   }

   tgl_assert(C_PuiSvHandler::h_GetInstance()->ClearNodeUpdateInformationAsAppropriate(this->mu32_ViewIndex,
                                                                                       this->mu32_NodeIndex,
                                                                                       C_PuiSvNodeUpdate::
                                                                                       eFTP_DATA_BLOCK) == C_NO_ERR);

   if (this->mq_FileBased == true)
   {
      tgl_assert(C_PuiSvHandler::h_GetInstance()->ClearNodeUpdateInformationAsAppropriate(this->mu32_ViewIndex,
                                                                                          this->mu32_NodeIndex,
                                                                                          C_PuiSvNodeUpdate::
                                                                                          eFTP_FILE_BASED) == C_NO_ERR);
   }

   // update visibility of optional sections
   this->UpdateSectionsVisibility();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepares the update package node configuration

   \param[out]     orc_NodeConfig         Node configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::PrepareExportConfig(C_SyvUpUpdatePackageConfigNode & orc_NodeConfig) const
{
   orc_NodeConfig.c_Name = this->mc_NodeName;
   orc_NodeConfig.c_DeviceType = this->mc_DeviceType;

   this->mpc_Ui->pc_DatablockWidget->PrepareExportConfig(orc_NodeConfig);

   if (this->mq_StwFlashloader == false)
   {
      this->mpc_Ui->pc_ParamsetWidget->PrepareExportConfig(orc_NodeConfig);
   }

   if (this->mq_FileBased == true)
   {
      this->mpc_Ui->pc_FileBasedWidget->PrepareExportConfig(orc_NodeConfig);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Loads all matching configurations for this node of the import configuration

   \param[out]     orc_Config         Import configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::LoadImportConfig(const C_SyvUpUpdatePackageConfig & orc_Config) const
{
   this->mpc_Ui->pc_DatablockWidget->LoadImportConfig(orc_Config);

   if (this->mq_StwFlashloader == false)
   {
      this->mpc_Ui->pc_ParamsetWidget->LoadImportConfig(orc_Config);
   }

   if (this->mq_FileBased == true)
   {
      this->mpc_Ui->pc_FileBasedWidget->LoadImportConfig(orc_Config);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks all paths for existence

   \param[out]    oru32_CountFiles           Number of files
   \param[out]    opc_FlashwareWarningsApps  App names of apps with flashware warnings
   \param[out]    opc_MissingDataBlocks      App names of apps with missing data block files
   \param[out]    opc_MissingParamFiles      App names of apps with missing parameter set image files
   \param[out]    opc_MissingFiles           App names of apps with missing files

   \return
   C_NO_ERR    All files are existent
   C_CONFIG    Minimum one file was not found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvUpUpdatePackageNodeWidget::CheckAllFiles(stw_types::uint32 & oru32_CountFiles,
                                                     QStringList * const opc_FlashwareWarningsApps,
                                                     QStringList * const opc_MissingApps,
                                                     QStringList * const opc_MissingParamFiles,
                                                     QStringList * const opc_MissingFiles) const
{
   sint32 s32_Return;

   s32_Return = this->mpc_Ui->pc_DatablockWidget->CheckAllFiles(oru32_CountFiles, opc_MissingApps,
                                                                opc_FlashwareWarningsApps);
   s32_Return += this->mpc_Ui->pc_ParamsetWidget->CheckAllFiles(oru32_CountFiles, opc_MissingParamFiles,
                                                                opc_FlashwareWarningsApps);

   if (this->mq_FileBased == true)
   {
      s32_Return += this->mpc_Ui->pc_FileBasedWidget->CheckAllFiles(oru32_CountFiles, opc_MissingFiles,
                                                                    opc_FlashwareWarningsApps);
   }

   if (s32_Return != C_NO_ERR)
   {
      s32_Return = C_CONFIG;
   }

   this->mpc_Ui->pc_LabIconWarning->setVisible(s32_Return != C_NO_ERR);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the section

   \param[in]     orc_Pos     Position of application

   \retval   NULL            No section list on the position
   \retval   Valid pointer   Pointer to section list
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpUpdatePackageListNodeWidget * C_SyvUpUpdatePackageNodeWidget::GetSectionList(const QPoint & orc_Pos) const
{
   C_SyvUpUpdatePackageListNodeWidget * pc_Return = NULL;
   const QPoint c_AdaptedPosScrollArea = this->mpc_Ui->pc_ScrollAreaWidget->mapFrom(this->parentWidget(), orc_Pos);

   QPoint c_AdaptedPosList = this->mpc_Ui->pc_DatablockWidget->mapFrom(this->mpc_Ui->pc_ScrollAreaWidget,
                                                                       c_AdaptedPosScrollArea);

   //QPoint c_AdaptedPosList = c_AdaptedPosScrollArea;
   if (this->mpc_Ui->pc_DatablockWidget->rect().contains(c_AdaptedPosList) == true)
   {
      pc_Return = this->mpc_Ui->pc_DatablockWidget;
   }
   else
   {
      c_AdaptedPosList = this->mpc_Ui->pc_ParamsetWidget->mapFrom(this->mpc_Ui->pc_ScrollAreaWidget,
                                                                  c_AdaptedPosScrollArea);
      //c_AdaptedPosList = c_AdaptedPosScrollArea;
      if (this->mpc_Ui->pc_ParamsetWidget->rect().contains(c_AdaptedPosList) == true)
      {
         pc_Return = this->mpc_Ui->pc_ParamsetWidget;
      }
      else
      {
         c_AdaptedPosList = this->mpc_Ui->pc_FileBasedWidget->mapFrom(this->mpc_Ui->pc_ScrollAreaWidget,
                                                                      c_AdaptedPosScrollArea);
         //c_AdaptedPosList = c_AdaptedPosScrollArea;
         if (this->mpc_Ui->pc_FileBasedWidget->rect().contains(c_AdaptedPosList) == true)
         {
            pc_Return = this->mpc_Ui->pc_FileBasedWidget;
         }
      }
   }

   return pc_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns and select the application on the position

   \param[in]     orc_Pos     Position of application

   \return
   Pointer to application
   NULL if no application was at the position
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpUpdatePackageListNodeItemWidget * C_SyvUpUpdatePackageNodeWidget::GetAndSelectApplication(const QPoint & orc_Pos)
const
{
   const QPoint c_AdaptedPos = this->mpc_Ui->pc_ScrollAreaWidget->mapFrom(this->parentWidget(), orc_Pos);

   C_SyvUpUpdatePackageListNodeItemWidget * pc_App = this->mpc_Ui->pc_DatablockWidget->GetAndSelectApplication(
      c_AdaptedPos);

   if (pc_App == NULL)
   {
      if (this->mq_StwFlashloader == false)
      {
         pc_App = this->mpc_Ui->pc_ParamsetWidget->GetAndSelectApplication(c_AdaptedPos);
      }

      if ((pc_App == NULL) &&
          (this->mq_FileBased == true))
      {
         pc_App = this->mpc_Ui->pc_FileBasedWidget->GetAndSelectApplication(c_AdaptedPos);
      }
   }

   return pc_App;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the application on the position

   \param[in]     orc_Pos        Position of application
   \param[out]    opu32_Number   Optional parameter for found app number

   \return
   Pointer to application
   NULL if no application was at the position
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpUpdatePackageListNodeItemWidget * C_SyvUpUpdatePackageNodeWidget::GetApplication(const QPoint & orc_Pos,
                                                                                        uint32 * const opu32_Number)
const
{
   const QPoint c_AdaptedPos = this->mpc_Ui->pc_ScrollAreaWidget->mapFrom(this->parentWidget(), orc_Pos);

   C_SyvUpUpdatePackageListNodeItemWidget * pc_App = this->mpc_Ui->pc_DatablockWidget->GetApplication(c_AdaptedPos,
                                                                                                      opu32_Number);

   if (pc_App == NULL)
   {
      if (this->mq_StwFlashloader == false)
      {
         pc_App = this->mpc_Ui->pc_ParamsetWidget->GetApplication(c_AdaptedPos, opu32_Number);
      }

      if ((pc_App == NULL) &&
          (this->mq_FileBased == true))
      {
         pc_App = this->mpc_Ui->pc_FileBasedWidget->GetApplication(c_AdaptedPos, opu32_Number);
      }
   }

   return pc_App;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the node index

   \return
   Current node index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpUpdatePackageNodeWidget::GetNodeIndex(void) const
{
   return this->mu32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns view index

   \return
   View index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpUpdatePackageNodeWidget::GetViewIndex(void) const
{
   return this->mu32_ViewIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the node name

   possible return value(s) and description
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpUpdatePackageNodeWidget::GetNodeName(void) const
{
   return this->mc_NodeName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the current position number

   \return
   Current position number
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpUpdatePackageNodeWidget::GetPositionNumber(void) const
{
   return this->mu32_PositionNumber;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the flag if the node is a file based device

   \return
   true     Node is file based
   false    Node is not file based
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdatePackageNodeWidget::IsFileBased(void) const
{
   return this->mq_FileBased;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the flag if the node has a STW flashloader

   \return
   true     Node has STW flashloader
   false    Node has not STW flashloader
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdatePackageNodeWidget::IsStwFlashloader(void) const
{
   return this->mq_StwFlashloader;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates and returns the update package with all information for the system update for the concrete node

   \param[out]    orc_ApplicationsToWrite       Vector with node update configuration
   \param[out]    orc_ApplicationsToWrite       Optional vector with all node applications

   \return
   C_NO_ERR    Update package with all information created
   C_RD_WR     At least one file does not exist
   C_NOACT     No files for applications to write added
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvUpUpdatePackageNodeWidget::GetUpdatePackage(C_OSCSuSequences::C_DoFlash & orc_ApplicationsToWrite,
                                                        C_OSCSuSequences::C_DoFlash * const opc_AllApplications)
{
   sint32 s32_Return;
   bool q_NoErr = false;

   this->mu32_FilesUpdated = 0U;

   s32_Return = this->mpc_Ui->pc_DatablockWidget->GetUpdatePackage(orc_ApplicationsToWrite, opc_AllApplications,
                                                                   this->mu32_FilesUpdated);

   if (s32_Return == C_NO_ERR)
   {
      q_NoErr = true;
   }

   if (((s32_Return == C_NO_ERR) ||
        (s32_Return == C_NOACT)) &&
       (this->mq_StwFlashloader == false))
   {
      s32_Return = this->mpc_Ui->pc_ParamsetWidget->GetUpdatePackage(orc_ApplicationsToWrite, opc_AllApplications,
                                                                     this->mu32_FilesUpdated);
      if (s32_Return == C_NO_ERR)
      {
         q_NoErr = true;
      }
   }

   if (((s32_Return == C_NO_ERR) ||
        (s32_Return == C_NOACT)) &&
       (this->mq_FileBased == true))
   {
      s32_Return = this->mpc_Ui->pc_FileBasedWidget->GetUpdatePackage(orc_ApplicationsToWrite, opc_AllApplications,
                                                                      this->mu32_FilesUpdated);
   }

   if ((q_NoErr == true) &&
       (s32_Return == C_NOACT))
   {
      // C_NOACT is no error if at least one GetUpdatePackage call returned with C_NO_ERR
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Collapsing all sections

   View for delegate
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::CollapseAll(void) const
{
   this->mpc_Ui->pc_ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   this->mpc_Ui->pc_DatablockWidget->Expand(false);
   this->mpc_Ui->pc_ParamsetWidget->Expand(false);
   this->mpc_Ui->pc_FileBasedWidget->Expand(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief Set empty optional sections visible flag and update visibility.

   \param[in]  oq_Visible  true: show sections   false: hide sections
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::SetEmptyOptionalSectionsVisible(const bool oq_Visible)
{
   // remember
   this->mq_EmptyOptionalSectionsVisible = oq_Visible;

   // update visibility
   this->UpdateSectionsVisibility();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief Update visibility of sections

   Show or hide sections "Parameter Set Images" and "Files" if there are no files in this sections.
   Do not touch sections that contain files.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::UpdateSectionsVisibility(void) const
{
   // parameter set images
   if (this->mq_StwFlashloader == false) // never visible for STW Flashloader
   {
      if (this->mpc_Ui->pc_ParamsetWidget->GetFileCount() == 0)
      {
         this->mpc_Ui->pc_ParamsetWidget->setVisible(this->mq_EmptyOptionalSectionsVisible);
      }
      else
      {
         this->mpc_Ui->pc_ParamsetWidget->setVisible(true);
      }
   }

   // files
   if (this->mq_FileBased == true) // never visible for non-file-based node
   {
      if (this->mpc_Ui->pc_FileBasedWidget->GetFileCount() == 0)
      {
         this->mpc_Ui->pc_FileBasedWidget->setVisible(this->mq_EmptyOptionalSectionsVisible);
      }
      else
      {
         this->mpc_Ui->pc_FileBasedWidget->setVisible(true);
      }
   }

   // separators
   this->mpc_Ui->pc_FrameSepMiddle->setVisible(
      (this->mpc_Ui->pc_DatablockWidget->isVisibleTo(this->mpc_Ui->pc_ScrollAreaWidget)) &&
      (this->mpc_Ui->pc_ParamsetWidget->isVisibleTo(this->mpc_Ui->pc_ScrollAreaWidget)));
   this->mpc_Ui->pc_FrameSepBottom->setVisible(
      ((this->mpc_Ui->pc_ParamsetWidget->isVisibleTo(this->mpc_Ui->pc_ScrollAreaWidget)) &&
       (this->mpc_Ui->pc_FileBasedWidget->isVisibleTo(this->mpc_Ui->pc_ScrollAreaWidget))) ||
      ((this->mpc_Ui->pc_DatablockWidget->isVisibleTo(this->mpc_Ui->pc_ScrollAreaWidget)) &&
       (this->mpc_Ui->pc_FileBasedWidget->isVisibleTo(this->mpc_Ui->pc_ScrollAreaWidget))));

   // label "no files"
   this->mpc_Ui->pc_WidgetAdd->setVisible(
      ((!(this->mpc_Ui->pc_DatablockWidget->isVisibleTo(this->mpc_Ui->pc_ScrollAreaWidget))) &&
       (!(this->mpc_Ui->pc_ParamsetWidget->isVisibleTo(this->mpc_Ui->pc_ScrollAreaWidget)))) &&
      (!(this->mpc_Ui->pc_FileBasedWidget->isVisibleTo(this->mpc_Ui->pc_ScrollAreaWidget))));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: Adapt the icon position and layout state

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);

   if (this->height() < mhs32_LAYOUT_THRESHOLD)
   {
      this->mpc_Ui->pc_ScrollArea->setVisible(false);
      this->mpc_Ui->pc_FrameSepTop->setVisible(false);
      this->mpc_Ui->pc_WidgetTitle->setMinimumHeight(30);
   }
   else
   {
      this->mpc_Ui->pc_ScrollArea->setVisible(true);
      this->mpc_Ui->pc_FrameSepTop->setVisible(true);
      this->mpc_Ui->pc_WidgetTitle->setMinimumHeight(50);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drag enter event slot

   Here: Accept external files

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::dragEnterEvent(QDragEnterEvent * const opc_Event)
{
   if (this->mq_Connected == false)
   {
      const QMimeData * const pc_MimeData = opc_Event->mimeData();

      if (pc_MimeData->hasUrls() == true)
      {
         opc_Event->accept();
      }
   }
   else
   {
      const QSize c_Size(36, 36);

      // Accept always to get the dragLeaveEvent to reset the size of the lock icon
      opc_Event->accept();

      this->mpc_Ui->pc_LabIconLock->setMinimumSize(c_Size);
      this->mpc_Ui->pc_LabIconLock->setMaximumSize(c_Size);
   }

   QWidget::dragEnterEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drag move event slot

   Here: Accept external files

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::dragMoveEvent(QDragMoveEvent * const opc_Event)
{
   if (this->mq_Connected == false)
   {
      const QMimeData * const pc_MimeData = opc_Event->mimeData();

      if (pc_MimeData->hasUrls() == true)
      {
         const QPoint c_AdaptedPos = this->mpc_Ui->pc_ScrollAreaWidget->mapFrom(this, opc_Event->pos());

         // Get Application expects coordinate from the parent of this widget
         if (this->m_CheckMime(pc_MimeData, c_AdaptedPos) == true)
         {
            opc_Event->accept();
         }
         else
         {
            opc_Event->ignore();
         }
      }
   }
   else
   {
      opc_Event->ignore();
   }

   QWidget::dragMoveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drag leave event slot

   Here: Deactivate selection of apps

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::dragLeaveEvent(QDragLeaveEvent * const opc_Event)
{
   const QSize c_Size(24, 24);

   this->mpc_Ui->pc_LabIconLock->setMinimumSize(c_Size);
   this->mpc_Ui->pc_LabIconLock->setMaximumSize(c_Size);

   this->m_SetApplicationsUnselected(this->mpc_Ui->pc_DatablockWidget);
   this->m_SetApplicationsUnselected(this->mpc_Ui->pc_FileBasedWidget);
   this->m_SetApplicationsUnselected(this->mpc_Ui->pc_ParamsetWidget);

   QWidget::dragLeaveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drop event slot

   Here: Handle dropped files

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::dropEvent(QDropEvent * const opc_Event)
{
   if (this->mq_Connected == false)
   {
      const QMimeData * const pc_MimeData = opc_Event->mimeData();
      QStringList c_FilePathsDatablocks;
      QStringList c_FilePathsParamsetFiles;
      QStringList c_FilePathsFileBased;
      C_SyvUpUpdatePackageListNodeItemWidget * pc_App = NULL;
      const QPoint c_AdaptedPos = this->mpc_Ui->pc_ScrollAreaWidget->mapFrom(this, opc_Event->pos());

      if (this->m_CheckMime(pc_MimeData, c_AdaptedPos, &c_FilePathsDatablocks,
                            &c_FilePathsParamsetFiles,
                            &c_FilePathsFileBased, &pc_App) == true)
      {
         const QString c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();

         if ((pc_App != NULL) &&
             (c_FilePathsDatablocks.size() == 1))
         {
            // Datablock only

            // Check if relative paths are possible and appreciated
            c_FilePathsDatablocks[0] =
               C_ImpUtil::h_AskUserToSaveRelativePath(this, c_FilePathsDatablocks[0], c_Folder);

            this->AdaptFile(c_FilePathsDatablocks[0], pc_App);
         }
         else
         {
            // Param sets and file based entries

            // Check if relative paths are possible and appreciated (for all files ask once)
            QStringList c_PathsParamAndFile;
            // Concatenate paths
            c_PathsParamAndFile.append(c_FilePathsParamsetFiles);
            c_PathsParamAndFile.append(c_FilePathsFileBased);
            // Ask
            c_PathsParamAndFile = C_ImpUtil::h_AskUserToSaveRelativePath(this, c_PathsParamAndFile, c_Folder);
            // Re-split paths
            tgl_assert(c_PathsParamAndFile.size() == (c_FilePathsParamsetFiles.size() + c_FilePathsFileBased.size()));
            c_FilePathsParamsetFiles = c_PathsParamAndFile.mid(0, c_FilePathsParamsetFiles.size());
            c_FilePathsFileBased = c_PathsParamAndFile.mid(c_FilePathsParamsetFiles.size(), -1);

            if ((pc_App != NULL) &&
                (c_FilePathsFileBased.size() == 1) &&
                (c_FilePathsParamsetFiles.size() == 0))
            {
               // Replace one file based entry
               this->AdaptFile(c_FilePathsFileBased[0], pc_App);
            }
            else if ((pc_App != NULL) &&
                     (c_FilePathsFileBased.size() == 0) &&
                     (c_FilePathsParamsetFiles.size() == 1))
            {
               // Replace one parameter set image entry
               this->AdaptFile(c_FilePathsParamsetFiles[0], pc_App);
            }
            else
            {
               sintn sn_FileCounter;
               // Add all files as new
               for (sn_FileCounter = 0; sn_FileCounter < c_FilePathsFileBased.size(); ++sn_FileCounter)
               {
                  // Add new file. If list does not support adding new files, nothing will happen
                  this->AddNewFile(c_FilePathsFileBased[sn_FileCounter], false);
               }
               for (sn_FileCounter = 0; sn_FileCounter < c_FilePathsParamsetFiles.size(); ++sn_FileCounter)
               {
                  // Add new file. If list does not support adding new files, nothing will happen
                  this->AddNewFile(c_FilePathsParamsetFiles[sn_FileCounter], true);
               }
            }
         }

         if (pc_App != NULL)
         {
            pc_App->SetSelected(false);
         }
      }
   }

   QWidget::dropEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::m_Init(void)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      tgl_assert(pc_Node->pc_DeviceDefinition != NULL);
      this->mq_FileBased = pc_Node->pc_DeviceDefinition->q_FlashloaderOpenSydeIsFileBased;
      this->mq_StwFlashloader = (pc_Node->c_Properties.e_FlashLoader == C_OSCNodeProperties::eFL_STW);
      this->mc_DeviceType = pc_Node->c_DeviceType.c_str();

      // Only relevant if the node has datablocks
      if (pc_Node->c_Applications.size() > 0)
      {
         this->mpc_Ui->pc_DatablockWidget->InitWidget(this->mu32_ViewIndex, this->mu32_PositionNumber,
                                                      this->mu32_NodeIndex, this->mc_NodeName);
      }
      this->mpc_Ui->pc_DatablockWidget->setVisible(pc_Node->c_Applications.size() > 0);

      // The second list with parameter set files is only available for openSYDE devices
      if (this->mq_StwFlashloader == false)
      {
         this->mpc_Ui->pc_ParamsetWidget->InitWidget(this->mu32_ViewIndex, this->mu32_PositionNumber,
                                                     this->mu32_NodeIndex, this->mc_NodeName);
      }
      this->mpc_Ui->pc_ParamsetWidget->setVisible(!this->mq_StwFlashloader);

      // The third list is only necessary for file based devices
      if (this->mq_FileBased == true)
      {
         this->mpc_Ui->pc_FileBasedWidget->InitWidget(this->mu32_ViewIndex, this->mu32_PositionNumber,
                                                      this->mu32_NodeIndex, this->mc_NodeName);
      }
      this->mpc_Ui->pc_FileBasedWidget->setVisible(this->mq_FileBased);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update of the title label
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::m_UpdateTitle(void) const
{
   this->mpc_Ui->pc_Title->setText(QString("#%1 - %2").arg(QString::number(this->mu32_PositionNumber + 1U),
                                                           this->mc_NodeName));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the number of all files in all sections

   \return
   Number of files
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpUpdatePackageNodeWidget::m_GetFileCount(void) const
{
   uint32 u32_Count;

   u32_Count = this->mpc_Ui->pc_DatablockWidget->GetFileCount() + this->mpc_Ui->pc_ParamsetWidget->GetFileCount();

   if (this->mq_FileBased == true)
   {
      u32_Count += this->mpc_Ui->pc_FileBasedWidget->GetFileCount();
   }

   return u32_Count;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the current relevant list for the current updating/transferring file/application

  This function use the value mu32_FilesUpdated as reference. The current file shall not be calculated into
  mu32_FilesUpdated before the call of this function.

   \retval   Valid pointer    Pointer to the list, which has the current updated file/application
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpUpdatePackageListNodeWidget * C_SyvUpUpdatePackageNodeWidget::m_GetNextListInUpdateOrder(void) const
{
   C_SyvUpUpdatePackageListNodeWidget * pc_List = this->mpc_Ui->pc_DatablockWidget;
   uint32 u32_SumFiles = this->mpc_Ui->pc_DatablockWidget->GetFileCount();

   // More files updated than datablocks. Check next section.
   if (this->mu32_FilesUpdated >= u32_SumFiles)
   {
      if (this->mq_StwFlashloader == false)
      {
         bool q_CheckParamset = true;

         if (this->mq_FileBased == true)
         {
            u32_SumFiles += this->mpc_Ui->pc_FileBasedWidget->GetFileCount();

            if (this->mu32_FilesUpdated < u32_SumFiles)
            {
               // Current file is in file based section
               pc_List = this->mpc_Ui->pc_FileBasedWidget;
               q_CheckParamset = false;
            }
         }

         if (q_CheckParamset == true)
         {
            u32_SumFiles += this->mpc_Ui->pc_ParamsetWidget->GetFileCount();

            if (this->mu32_FilesUpdated < u32_SumFiles)
            {
               pc_List = this->mpc_Ui->pc_ParamsetWidget;
            }
            else
            {
               // Shall not happen
               tgl_assert(false);
            }
         }
      }
      else
      {
         // Shall not happen
         tgl_assert(false);
      }
   }

   return pc_List;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets all applications of the specific list unselected

   \param[in]       opc_List    List with application to adapt
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageNodeWidget::m_SetApplicationsUnselected(
   const C_SyvUpUpdatePackageListNodeWidget * const opc_List) const
{
   uint32 u32_AppCounter;

   for (u32_AppCounter = 0U; u32_AppCounter < opc_List->GetFileCount(); ++u32_AppCounter)
   {
      opc_List->SetApplicationSelect(u32_AppCounter, false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the list parent of a specific app

   \param[in]     opc_App       Application widget

   \return
   Valid pointer     Pointer to list widget with this app as child
   NULL              No parent was found
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpUpdatePackageListNodeWidget * C_SyvUpUpdatePackageNodeWidget::m_GetAppParentList(
   C_SyvUpUpdatePackageListNodeItemWidget * const opc_App) const
{
   C_SyvUpUpdatePackageListNodeWidget * pc_Parent = NULL;

   if (opc_App != NULL)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      pc_Parent = dynamic_cast<C_SyvUpUpdatePackageListNodeWidget *>(opc_App->GetListParent());

      tgl_assert(pc_Parent != NULL);
   }

   return pc_Parent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Compare all files in sections data blocks and files with input file.

   If file is already contained in update package, an information message is shown.

   \retval   true    File is already contained
   \retval   false   File is new
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdatePackageNodeWidget::m_CheckFileAlreadyContained(const QString & orc_File)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   bool q_Retval = false;

   if ((pc_Node != NULL) &&
       (pc_View != NULL))
   {
      const QString c_AbsoluteFile = C_ImpUtil::h_GetAbsolutePathFromProject(orc_File);
      std::vector<QString> c_Paths;

      // compare with existing data block files
      c_Paths = pc_View->GetNodeUpdateInformation(this->mu32_NodeIndex)->GetPaths(C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
      for (std::vector<QString>::const_iterator c_It = c_Paths.begin(); c_It != c_Paths.end(); ++c_It)
      {
         if (c_AbsoluteFile == C_ImpUtil::h_GetAbsolutePathFromProject(*c_It))
         {
            q_Retval = true;
            break;
         }
      }

      // compare with existing file based files
      if (q_Retval == false)
      {
         c_Paths =
            pc_View->GetNodeUpdateInformation(this->mu32_NodeIndex)->GetPaths(C_PuiSvNodeUpdate::eFTP_FILE_BASED);
         for (std::vector<QString>::const_iterator c_It = c_Paths.begin(); c_It != c_Paths.end(); ++c_It)
         {
            if (c_AbsoluteFile == C_ImpUtil::h_GetAbsolutePathFromProject(*c_It))
            {
               q_Retval = true;
               break;
            }
         }
      }
   }

   if (q_Retval == true)
   {
      // inform user that file already exists
      C_OgeWiCustomMessage c_Message(this);
      c_Message.SetHeading(C_GtGetText::h_GetText("Add file"));
      c_Message.SetDescription(QString(C_GtGetText::h_GetText("The file %1 is already contained in the Update Package "
                                                              "for this node and therefore not added again.")).
                               arg(C_ImpUtil::h_GetAbsolutePathFromProject(orc_File)));
      c_Message.Execute();
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if mime valid

   \param[in]     opc_Mime     Mime to check
   \param[in]     orc_Pos      Mouse position
   \param[out]    opc_FilePathsDatablocks    File paths which could be used for datablock list
   \param[out]    opc_FilePathsParamsets     File paths which could be used for parameter set image list
   \param[out]    opc_FilePathsFileBased     File paths which could be used for file based list
   \param[out]    oppc_App     Found application widget

   \return
   true  Valid
   false Invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdatePackageNodeWidget::m_CheckMime(const QMimeData * const opc_Mime, const QPoint & orc_Pos,
                                                 QStringList * const opc_FilePathsDatablocks,
                                                 QStringList * const opc_FilePathsParamsets,
                                                 QStringList * const opc_FilePathsFileBased,
                                                 C_SyvUpUpdatePackageListNodeItemWidget ** const oppc_App) const
{
   bool q_Retval = false;

   // Check the mime parameter
   if ((opc_Mime != NULL) &&
       (opc_Mime->hasUrls() == true))
   {
      QStringList c_PathList;
      QList<QUrl> c_UrlList = opc_Mime->urls();
      QFileInfo c_File;
      sintn sn_FileCounter;
      bool q_FilesValid = true;

      // extract the local paths of the files
      for (sintn sn_It = 0; sn_It < c_UrlList.size(); ++sn_It)
      {
         c_PathList.append(c_UrlList.at(sn_It).toLocalFile());
      }

      // Check all paths
      for (sn_FileCounter = 0U; sn_FileCounter < c_PathList.size(); ++sn_FileCounter)
      {
         c_File.setFile(c_PathList[sn_FileCounter]);
         if ((c_File.exists() == false) || (c_File.isFile() == false))
         {
            q_FilesValid = false;
            break;
         }
      }

      if (q_FilesValid == true)
      {
         // Check all possible types
         q_Retval = this->mpc_Ui->pc_DatablockWidget->CheckMime(c_PathList, orc_Pos, opc_FilePathsDatablocks, oppc_App);

         // If datablocks are possible, no other list is possible
         if (q_Retval == false)
         {
            if (this->mq_StwFlashloader == false)
            {
               q_Retval =
                  this->mpc_Ui->pc_ParamsetWidget->CheckMime(c_PathList, orc_Pos, opc_FilePathsParamsets, oppc_App);
            }

            if (((q_Retval == false) || (opc_FilePathsFileBased != NULL)) &&
                (this->mq_FileBased == true))
            {
               // Check if no parameter set image operation is possible or the caller wants to know all possibilities
               // In this case both checks are necessary
               q_Retval =
                  this->mpc_Ui->pc_FileBasedWidget->CheckMime(c_PathList, orc_Pos, opc_FilePathsFileBased, oppc_App) ||
                  q_Retval;
            }
         }
      }
   }

   return q_Retval;
}
