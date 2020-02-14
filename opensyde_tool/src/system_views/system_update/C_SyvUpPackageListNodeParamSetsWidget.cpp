//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for listing and handling paramset files

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QFileInfo>

#include "stwtypes.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_SyvUpPackageListNodeParamSetsWidget.h"
#include "ui_C_SyvUpUpdatePackageListNodeWidget.h"
#include "C_PuiSvNodeUpdateParamInfo.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_PuiUtil.h"
#include "C_OgeWiCustomMessage.h"
#include "C_OgePopUpDialog.h"
#include "C_PuiSvHandler.h"
#include "C_OSCLoggingHandler.h"

#include "C_SyvUpPackageListNodeItemParamSetWidget.h"
#include "C_SyvUpParamSetFileAddPopUp.h"
#include "C_OSCSuSequences.h"

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

   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPackageListNodeParamSetsWidget::C_SyvUpPackageListNodeParamSetsWidget(QWidget * const opc_Parent) :
   C_SyvUpUpdatePackageListNodeWidget(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the entire update was finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeParamSetsWidget::SetUpdateFinished(void) const
{
   uint32 u32_Counter;

   // Special case: After updating a parameter set file, it is not possible to read back or confirm
   // the state of the node. Reset the state of all parameter set files no matter
   // whether it is a file or address based node
   for (u32_Counter = 0U; u32_Counter < this->mu32_FileCount; ++u32_Counter)
   {
      this->m_SetApplicationState(u32_Counter, C_SyvUpUpdatePackageListNodeItemWidget::hu32_STATE_TO_DO);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new file for the file container

   Only possible for openSYDE devices

   \param[in]     orc_File      New path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeParamSetsWidget::AddFile(const QString & orc_File)
{
   if (this->mq_StwFlashloader == false)
   {
      C_PuiSvNodeUpdateParamInfo c_ParamFileInfo;
      if (this->GetParamsetFileInfo(orc_File, c_ParamFileInfo) == C_NO_ERR)
      {
         C_SyvUpPackageListNodeItemParamSetWidget * const pc_AppWidget =
            new C_SyvUpPackageListNodeItemParamSetWidget(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                         this->mc_DeviceType,
                                                         this->mq_FileBased,
                                                         this->mq_StwFlashloader,
                                                         this);

         // add file
         QFileInfo c_File(orc_File);

         // Pre initialization of the size. If this is not set, the eliding of the path label will cause a
         // visible resizing
         pc_AppWidget->setGeometry(this->width(), 40, 0, 0);

         pc_AppWidget->SetParamInfo(c_ParamFileInfo);
         pc_AppWidget->SetAppName(c_File.fileName());
         pc_AppWidget->SetAppNumber(static_cast<uint32>(this->mpc_Ui->pc_AppVerticalLayout->count() - 1));

         this->mpc_Ui->pc_AppVerticalLayout->insertWidget(this->mpc_Ui->pc_AppVerticalLayout->count() - 1,
                                                          pc_AppWidget);

         // Save the new file as application path
         tgl_assert(C_PuiSvHandler::h_GetInstance()->AddNodeUpdateInformationParamInfo(
                       this->mu32_ViewIndex,
                       this->mu32_NodeIndex,
                       c_ParamFileInfo) == C_NO_ERR);

         this->mpc_Ui->pc_WidgetAdd->setVisible(false);
         ++this->mu32_FileCount;

         this->m_FileCountChanged();
         //lint -e{429}  no memory leak because of the parent of pc_AppWidget and the Qt memory management
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new file for the application

   \param[in]     orc_File      New path
   \param[in]     opc_App       Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeParamSetsWidget::AdaptFile(const QString & orc_File,
                                                      C_SyvUpUpdatePackageListNodeItemWidget * const opc_App)
{
   if (opc_App != NULL)
   {
      C_PuiSvNodeUpdateParamInfo c_ParamFileInfo;
      if (this->GetParamsetFileInfo(orc_File, c_ParamFileInfo) == C_NO_ERR)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SyvUpPackageListNodeItemParamSetWidget * const pc_AppWidget =
            dynamic_cast<C_SyvUpPackageListNodeItemParamSetWidget *>(opc_App);

         C_SyvUpUpdatePackageListNodeWidget::AdaptFile(orc_File, opc_App);

         tgl_assert(pc_AppWidget != NULL);
         if (pc_AppWidget != NULL)
         {
            QFileInfo c_File(orc_File);

            pc_AppWidget->SetAppName(c_File.fileName());
            pc_AppWidget->SetParamInfo(c_ParamFileInfo);
         }

         tgl_assert(C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformationParamInfo(this->mu32_ViewIndex,
                                                                                       this->mu32_NodeIndex,
                                                                                       opc_App->GetAppNumber(),
                                                                                       c_ParamFileInfo) == C_NO_ERR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes the current application

   \param[in]     opc_App       Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeParamSetsWidget::RemoveFile(C_SyvUpUpdatePackageListNodeItemWidget * const opc_App)
{
   if (opc_App != NULL)
   {
      const uint32 u32_ParamNumber = opc_App->GetAppNumber();

      // Remove the application file path
      tgl_assert(C_PuiSvHandler::h_GetInstance()->RemoveNodeUpdateInformationParamInfo(this->mu32_ViewIndex,
                                                                                       this->mu32_NodeIndex,
                                                                                       u32_ParamNumber) == C_NO_ERR);
   }

   C_SyvUpUpdatePackageListNodeWidget::RemoveFile(opc_App);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepares the update package node configuration

   \param[out]     orc_NodeConfig         Node configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeParamSetsWidget::PrepareExportConfig(C_SyvUpUpdatePackageConfigNode & orc_NodeConfig) const
{
   // One spacer is in the layout and is not relevant
   if (this->mpc_Ui->pc_AppVerticalLayout->count() > 1)
   {
      sintn sn_Counter;
      const sintn sn_AppCount = this->mpc_Ui->pc_AppVerticalLayout->count() - 1;

      orc_NodeConfig.c_ParamSetConfigs.reserve(sn_AppCount);

      for (sn_Counter = 0; sn_Counter < this->mpc_Ui->pc_AppVerticalLayout->count(); ++sn_Counter)
      {
         QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_Counter);

         if (pc_Item != NULL)
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            C_SyvUpUpdatePackageListNodeItemWidget * const pc_App =
               dynamic_cast<C_SyvUpUpdatePackageListNodeItemWidget *>(pc_Item->widget());

            if (pc_App != NULL)
            {
               orc_NodeConfig.c_ParamSetConfigs.push_back(pc_App->GetAppFilePath());
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Loads all matching configurations for this node of the import configuration

   \param[out]     orc_Config         Import configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeParamSetsWidget::LoadImportConfig(const C_SyvUpUpdatePackageConfig & orc_Config)
{
   uint32 u32_ConfigCounter;

   // Search the matching configuration
   for (u32_ConfigCounter = 0; u32_ConfigCounter < orc_Config.c_NodeConfigs.size(); ++u32_ConfigCounter)
   {
      if ((orc_Config.c_NodeConfigs[u32_ConfigCounter].c_Name == this->mc_NodeName) &&
          (orc_Config.c_NodeConfigs[u32_ConfigCounter].c_DeviceType == this->mc_DeviceType))
      {
         uintn un_FileCounter;

         // Remove the previous applications
         this->RemoveAllFiles();

         // Add the imported configuration as new applications
         for (un_FileCounter = 0U;
              un_FileCounter < orc_Config.c_NodeConfigs[u32_ConfigCounter].c_ParamSetConfigs.size();
              ++un_FileCounter)
         {
            const QString c_Path = orc_Config.c_NodeConfigs[u32_ConfigCounter].c_ParamSetConfigs[un_FileCounter];
            QFileInfo c_File(c_Path);

            if (c_File.completeSuffix().toLower() == "syde_psi")
            {
               // Update UI and update view data handling
               this->AddFile(c_Path);
            }
         }

         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if mime valid

   File based list accept multiple files

   \param[in,out]   orc_PathList             File paths to analyze, relevant paths in opc_RelevantFilePaths in
                                             will be removed here
   \param[in]       orc_Pos                  Mouse position
   \param[out]      opc_RelevantFilePaths    File paths which could be used for this list
   \param[out]      oppc_App                 Found application widget

   \return
   true  Valid
   false Invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPackageListNodeParamSetsWidget::CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos,
                                                      QStringList * const opc_RelevantFilePaths,
                                                      C_SyvUpUpdatePackageListNodeItemWidget ** const oppc_App) const
{
   bool q_Retval = false;

   if (orc_PathList.size() > 0)
   {
      C_SyvUpUpdatePackageListNodeItemWidget * pc_App = NULL;

      q_Retval = C_SyvUpUpdatePackageListNodeWidget::CheckMime(orc_PathList, orc_Pos, opc_RelevantFilePaths, &pc_App);

      if (q_Retval == true)
      {
         sintn sn_Counter = 0;

         q_Retval = false;

         do
         {
            QFileInfo c_File(orc_PathList[sn_Counter]);

            if (c_File.completeSuffix().toLower() == "syde_psi")
            {
               // File is relevant
               q_Retval  = true;

               if (opc_RelevantFilePaths != NULL)
               {
                  // All relevant files must be copied
                  opc_RelevantFilePaths->push_back(orc_PathList[sn_Counter]);
                  orc_PathList.erase(orc_PathList.begin() + sn_Counter);
               }
               else
               {
                  // The information of the return value is relevant only
                  break;
               }
            }
            else
            {
               ++sn_Counter;
            }
         }
         while (sn_Counter < orc_PathList.size());

         if ((q_Retval == true) &&
             (oppc_App != NULL))
         {
            *oppc_App = pc_App;
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates and returns the update package with all information for the system update for the concrete node

   \param[out]    orc_ApplicationsToWrite     Vector with node update configuration
   \param[out]    opc_AllApplications         Optional vector with all node applications
   \param[out]    oru32_FilesUpdated          Number of already updated files or no update necessary

   \return
   C_NO_ERR    Update package with all information created
   C_RD_WR     At least one file does not exist
   C_NOACT     No files for applications to write added
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvUpPackageListNodeParamSetsWidget::GetUpdatePackage(C_OSCSuSequences::C_DoFlash & orc_ApplicationsToWrite,
                                                               C_OSCSuSequences::C_DoFlash * const opc_AllApplications,
                                                               stw_types::uint32 & oru32_FilesUpdated) const
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mq_StwFlashloader == false)
   {
      sintn sn_Counter;

      for (sn_Counter = 0; sn_Counter < this->mpc_Ui->pc_AppVerticalLayout->count(); ++sn_Counter)
      {
         QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_Counter);

         if (pc_Item != NULL)
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            C_SyvUpPackageListNodeItemParamSetWidget * const pc_App =
               dynamic_cast<C_SyvUpPackageListNodeItemParamSetWidget *>(pc_Item->widget());

            if (pc_App != NULL)
            {
               const QString c_Path = pc_App->GetAppAbsoluteFilePath();
               QFileInfo c_File(c_Path);

               // Check file
               if ((c_File.exists() == true) &&
                   (c_File.isFile() == true))
               {
                  const QString c_LogEntry =
                     "Generate Update Package: For node \"%1\" used parameter set image file: %2";

                  if (pc_App->GetState() != C_SyvUpUpdatePackageListNodeItemWidget::hu32_STATE_FINISHED)
                  {
                     orc_ApplicationsToWrite.c_FilesToWriteToNvm.push_back(stw_scl::C_SCLString(
                                                                              c_Path.toStdString().c_str()));
                  }
                  else
                  {
                     // Update of application is not necessary
                     ++oru32_FilesUpdated;
                  }

                  if (opc_AllApplications != NULL)
                  {
                     // Fill vector with all applications independent of the state
                     opc_AllApplications->c_FilesToWriteToNvm.push_back(stw_scl::C_SCLString(
                                                                           c_Path.toStdString().c_str()));
                  }

                  osc_write_log_info("Generate Update Package",
                                     C_GtGetText::h_GetText(c_LogEntry.arg(this->mc_NodeName,
                                                                           c_Path).toStdString().c_str()));
               }
               else
               {
                  const QString c_LogEntry =
                     "Generate Update Package: The path of Node \"%1\" parameter set image file is invalid: %2";

                  osc_write_log_info("Generate Update Package",
                                     C_GtGetText::h_GetText(c_LogEntry.arg(this->mc_NodeName,
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
   }

   if (orc_ApplicationsToWrite.c_FilesToWriteToNvm.size() == 0)
   {
      // No files added
      s32_Return = C_NOACT;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get type of subsection (data block/parameter set file/file based node file)

   \retval type parameter set file
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpPackageListNodeParamSetsWidget::Type(void) const
{
   return mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PARAMSET;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of the applications in case of datablock configuration

   \param[in]       orc_Node         Current node
   \param[in]       orc_UpdateInfo   Configured update configuration of view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeParamSetsWidget::m_InitSpecificItem(const stw_opensyde_core::C_OSCNode & orc_Node,
                                                               const stw_opensyde_gui_logic::C_PuiSvNodeUpdate & orc_UpdateInfo)
{
   const std::vector<C_PuiSvNodeUpdateParamInfo> c_ViewParamsetPaths = orc_UpdateInfo.GetParamInfos();
   uint32 u32_Counter;

   Q_UNUSED(orc_Node)

   for (u32_Counter = 0U; u32_Counter < c_ViewParamsetPaths.size(); ++u32_Counter)
   {
      C_PuiSvNodeUpdateParamInfo c_ParamInfo;
      const C_PuiSvNodeUpdateParamInfo & rc_FileInfo = c_ViewParamsetPaths[u32_Counter];
      C_SyvUpPackageListNodeItemParamSetWidget * const pc_AppWidget =
         new C_SyvUpPackageListNodeItemParamSetWidget(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                      this->mc_DeviceType,
                                                      this->mq_FileBased, this->mq_StwFlashloader, this);

      QFileInfo c_File(rc_FileInfo.GetPath());
      pc_AppWidget->SetAppFile(rc_FileInfo.GetPath(), false);
      pc_AppWidget->SetAppName(c_File.fileName());
      pc_AppWidget->SetAppNumber(u32_Counter);
      //Set initial param set info
      c_ParamInfo.SetContent(rc_FileInfo.GetPath(), rc_FileInfo.GetLastKnownCrc());
      pc_AppWidget->SetParamInfo(c_ParamInfo);

      ++this->mu32_FileCount;
      this->mpc_Ui->pc_AppVerticalLayout->addWidget(pc_AppWidget);
      //lint -e{429}  no memory leak because of the parent of pc_AppWidget and the Qt memory management
   }

   this->mq_ShowAddButton = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeParamSetsWidget::m_InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabTitle->setText(C_GtGetText::h_GetText("Parameter Set Images (optional)"));
   this->mpc_Ui->pc_LabIcon->SetSvg(":images/system_views/IconFileParamSetImage.svg");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Short function description

   \param[in]     orc_File          New path
   \param[out]    orc_ParamFileInfo Read parameter set file information

   \return
   C_NO_ERR    File read
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvUpPackageListNodeParamSetsWidget::GetParamsetFileInfo(const QString & orc_File,
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
         QString c_Details = QString(C_GtGetText::h_GetText("File path: %1")).
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
            c_Details += QString(C_GtGetText::h_GetText("Unknown reason. Error code: %1")).arg(s32_ReadFileResult);
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
      }
      //lint -e{429}  no memory leak because of the parent of pc_InfoDialog and the Qt memory management
   }
   else
   {
      // inform user that file already exists
      C_OgeWiCustomMessage c_Message(this);
      c_Message.SetHeading(C_GtGetText::h_GetText("Add file"));
      c_Message.SetDescription(
         QString(C_GtGetText::h_GetText("The file %1 is already contained in the Update Package for this node "
                                        "and therefore not added again.")).
         arg(C_PuiUtil::h_GetAbsolutePathFromProject(orc_File)));
      c_Message.Execute();
   }

   return s32_Return;
}
