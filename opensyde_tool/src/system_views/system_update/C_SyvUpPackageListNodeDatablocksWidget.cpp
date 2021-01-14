//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for listing and handling datablock files

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QFileInfo>

#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_Uti.h"
#include "C_PuiUtil.h"

#include "C_SyvUpPackageListNodeDatablocksWidget.h"
#include "ui_C_SyvUpUpdatePackageListNodeWidget.h"
#include "C_OSCNodeApplication.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_SyvUpPackageListNodeItemDatablockWidget.h"
#include "C_OgeWiCustomMessage.h"
#include "C_OsyHexFile.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvUpPackageListNodeDatablocksWidget::mhc_RemovePathText("<Add File>");

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
C_SyvUpPackageListNodeDatablocksWidget::C_SyvUpPackageListNodeDatablocksWidget(QWidget * const opc_Parent) :
   C_SyvUpUpdatePackageListNodeWidget(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new file for the application

   \param[in]  orc_File    New path (already contains Db Project path and might be relative to openSYDE project)
   \param[in]  opc_App     Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeDatablocksWidget::AdaptFile(const QString & orc_File,
                                                       C_SyvUpUpdatePackageListNodeItemWidget * const opc_App)
{
   bool q_FileIsOk = false;

   if (orc_File == C_GtGetText::h_GetText(mhc_RemovePathText.toStdString().c_str()))
   {
      // Special case. Removing file. No check necessary
      q_FileIsOk = true;
   }
   else
   {
      // Check the file
      // note: Data Block project path is already prefixed, so we only make absolute to openSYDE project
      const QString c_AbsoluteFilePath = C_PuiUtil::h_GetResolvedAbsPathFromProject(orc_File);
      QFileInfo c_FileInfo(c_AbsoluteFilePath);
      const bool q_FileExists = c_FileInfo.exists() && c_FileInfo.isFile();

      if (q_FileExists == true)
      {
         if (this->mq_FileBased == false)
         {
            C_OsyHexFile * const pc_HexFile = new C_OsyHexFile();
            if (pc_HexFile->LoadFromFile(c_AbsoluteFilePath.toStdString().c_str()) == stw_hex_file::NO_ERR)
            {
               stw_diag_lib::C_XFLECUInformation c_FileApplicationInfo;
               const sint32 s32_Result = pc_HexFile->ScanApplicationInformationBlockFromHexFile(c_FileApplicationInfo);

               if ((s32_Result == C_NO_ERR) || (s32_Result == C_WARN))
               {
                  const QString c_AppDeviceType = c_FileApplicationInfo.GetDeviceID().Trim().UpperCase().c_str();
                  // No check with Alias necessary due to check with real device type defined with target integration
                  if ((this->mq_StwFlashloader == true) ||
                      (QString::compare(this->mc_DeviceType.trimmed(), c_AppDeviceType, Qt::CaseInsensitive) == 0))
                  {
                     q_FileIsOk = true;
                  }
                  else
                  {
                     const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
                        this->mu32_NodeIndex);
                     if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL))
                     {
                        for (uint32 u32_ItName = 0UL;
                             u32_ItName < pc_Node->pc_DeviceDefinition->c_OtherAcceptedNames.size(); ++u32_ItName)
                        {
                           const QString c_AllowedDevice =
                              pc_Node->pc_DeviceDefinition->c_OtherAcceptedNames[u32_ItName].Trim().UpperCase().c_str();
                           if (QString::compare(c_AllowedDevice, c_AppDeviceType, Qt::CaseInsensitive) == 0)
                           {
                              q_FileIsOk = true;
                           }
                        }
                     }
                     if (q_FileIsOk == false)
                     {
                        C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
                        c_Message.SetHeading(C_GtGetText::h_GetText("Update Package Configuration"));
                        c_Message.SetDescription(C_GtGetText::h_GetText("Device type of selected HEX file does not "
                                                                        "match the node type."));
                        c_Message.SetDetails(
                           static_cast<QString>(C_GtGetText::h_GetText("Device type of %1 does not match node type %2."))
                           .arg(c_AppDeviceType.trimmed(), this->mc_DeviceType.trimmed()));
                        c_Message.SetCustomMinHeight(230, 250);
                        c_Message.Execute();
                     }
                  }
               }
               else
               {
                  // HEX file application block information is invalid
                  C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
                  c_Message.SetHeading(C_GtGetText::h_GetText("Update Package Configuration"));
                  if (s32_Result == C_OVERFLOW)
                  {
                     c_Message.SetDescription(C_GtGetText::h_GetText("HEX file has multiple application information "
                                                                     "blocks with non-equal device names!"));
                     c_Message.SetCustomMinHeight(180, 180);
                  }
                  else
                  {
                     c_Message.SetDescription(C_GtGetText::h_GetText("HEX file has no application information block!"));
                     c_Message.SetCustomMinHeight(180, 180);
                  }
                  c_Message.Execute();
               }
            }
            else
            {
               const QString c_Details = static_cast<QString>(C_GtGetText::h_GetText("File path: %1")).arg(c_AbsoluteFilePath);
               // Invalid file
               C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
               c_Message.SetHeading(C_GtGetText::h_GetText("Update Package configuration"));
               c_Message.SetDescription(C_GtGetText::h_GetText("File is not a valid HEX file!"));
               c_Message.SetDetails(c_Details);
               c_Message.SetCustomMinHeight(180, 250);
               c_Message.Execute();
            }

            delete pc_HexFile;
         }
         else
         {
            q_FileIsOk = true;
         }
      }
   }

   if ((q_FileIsOk == true) &&
       (opc_App != NULL))
   {
      C_SyvUpUpdatePackageListNodeWidget::AdaptFile(orc_File, opc_App);

      tgl_assert(C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformationPath(this->mu32_ViewIndex,
                                                                               this->mu32_NodeIndex,
                                                                               opc_App->GetAppNumber(),
                                                                               orc_File,
                                                                               C_PuiSvNodeUpdate::
                                                                               eFTP_DATA_BLOCK) == C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Revert the current application to the system definition default path

   Only for address based devices

   \param[in]  opc_App  Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeDatablocksWidget::RevertFile(C_SyvUpUpdatePackageListNodeItemWidget * const opc_App)
{
   // Reverting path only available for datablock applications
   if (opc_App != NULL)
   {
      uint32 u32_AppNumber = opc_App->GetAppNumber();
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      if (pc_Node != NULL)
      {
         // only revert if default application path is not empty
         if (pc_Node->c_Applications[u32_AppNumber].c_ResultPath != "")
         {
            // Restore the default path
            if ((u32_AppNumber < pc_Node->c_Applications.size()))
            {
               // Remove the view specific path
               tgl_assert(C_PuiSvHandler::h_GetInstance()->
                          SetNodeUpdateInformationPath(this->mu32_ViewIndex, this->mu32_NodeIndex, u32_AppNumber, "",
                                                       C_PuiSvNodeUpdate::eFTP_DATA_BLOCK) == C_NO_ERR);

               opc_App->SetAppFile(C_PuiUtil::h_MakeIndependentOfDbProjectPath(
                                      pc_Node->c_Applications[u32_AppNumber].c_ProjectPath.c_str(),
                                      pc_Node->c_Applications[u32_AppNumber].c_ResultPath.c_str()), true);
            }
         }
         else
         {
            C_OgeWiCustomMessage c_Message(this);
            c_Message.SetDescription(static_cast<QString>(C_GtGetText::h_GetText("No default output file declared in properties of "
                                                                    "Data Block \"%1\". ")).arg(opc_App->GetAppName()));
            c_Message.SetHeading(C_GtGetText::h_GetText("Revert to Default"));
            c_Message.Execute();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes the current application

   Not remove of datablock, adaption of the current associated file only

   \param[in]  opc_App  Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeDatablocksWidget::RemoveFile(C_SyvUpUpdatePackageListNodeItemWidget * const opc_App)
{
   this->AdaptFile(C_GtGetText::h_GetText(mhc_RemovePathText.toStdString().c_str()), opc_App);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepares the update package node configuration

   \param[out]  orc_NodeConfig   Node configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeDatablocksWidget::PrepareExportConfig(C_SyvUpUpdatePackageConfigNode & orc_NodeConfig) const
{
   // One spacer is in the layout and is not relevant
   if (this->mpc_Ui->pc_AppVerticalLayout->count() > 1)
   {
      sintn sn_Counter;
      const sintn sn_AppCount = this->mpc_Ui->pc_AppVerticalLayout->count() - 1;

      orc_NodeConfig.c_AppConfigs.reserve(sn_AppCount);

      for (sn_Counter = 0; sn_Counter < this->mpc_Ui->pc_AppVerticalLayout->count(); ++sn_Counter)
      {
         QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_Counter);

         if (pc_Item != NULL)
         {

            C_SyvUpUpdatePackageListNodeItemWidget * const pc_App =
               dynamic_cast<C_SyvUpUpdatePackageListNodeItemWidget *>(pc_Item->widget());

            if (pc_App != NULL)
            {
               C_SyvUpUpdatePackageConfigNodeApp c_AppConfig;

               pc_App->PrepareExportConfig(c_AppConfig);

               orc_NodeConfig.c_AppConfigs.push_back(c_AppConfig);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Loads all matching configurations for this node of the import configuration

   \param[out]  orc_Config    Import configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeDatablocksWidget::LoadImportConfig(const C_SyvUpUpdatePackageConfig & orc_Config)
{
   uint32 u32_ConfigCounter;

   // Search the matching configuration
   for (u32_ConfigCounter = 0; u32_ConfigCounter < orc_Config.c_NodeConfigs.size(); ++u32_ConfigCounter)
   {
      if ((orc_Config.c_NodeConfigs[u32_ConfigCounter].c_Name == this->mc_NodeName) &&
          (orc_Config.c_NodeConfigs[u32_ConfigCounter].c_DeviceType == this->mc_DeviceType))
      {
         sintn sn_AppCounter;
         // Matching configuration found. Load the configuration for the applications
         for (sn_AppCounter = 0; sn_AppCounter < this->mpc_Ui->pc_AppVerticalLayout->count(); ++sn_AppCounter)
         {
            QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_AppCounter);

            if (pc_Item != NULL)
            {

               C_SyvUpUpdatePackageListNodeItemWidget * const pc_App =
                  dynamic_cast<C_SyvUpUpdatePackageListNodeItemWidget *>(pc_Item->widget());

               if (pc_App != NULL)
               {
                  // Update UI and update view data handling
                  pc_App->LoadImportConfig(orc_Config.c_NodeConfigs[u32_ConfigCounter]);
               }
            }
         }

         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update device information

   Check and update of information which application is already finished and must not be updated

   \param[in]  orc_DeviceInformation   Device info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeDatablocksWidget::UpdateDeviceInformation(const C_SyvUpDeviceInfo & orc_DeviceInformation)
const
{
   uint32 u32_AppCountOnTarget;
   sintn sn_AppWidgetCounter;

   //get number of applications present on target
   if (orc_DeviceInformation.pc_OSYDevice != NULL)
   {
      u32_AppCountOnTarget = orc_DeviceInformation.pc_OSYDevice->c_Applications.size();
   }
   else if (orc_DeviceInformation.pc_STWDevice != NULL)
   {
      u32_AppCountOnTarget = orc_DeviceInformation.pc_STWDevice->c_BasicInformation.c_DeviceInfoBlocks.GetLength();
   }
   else
   {
      // Nothing to do
      u32_AppCountOnTarget = 0U;
   }
   //go through all applications in the UI
   for (sn_AppWidgetCounter = 0; sn_AppWidgetCounter < this->mpc_Ui->pc_AppVerticalLayout->count();
        ++sn_AppWidgetCounter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_AppWidgetCounter);

      if (pc_Item != NULL)
      {

         C_SyvUpUpdatePackageListNodeItemWidget * const pc_App =
            dynamic_cast<C_SyvUpUpdatePackageListNodeItemWidget *>(pc_Item->widget());

         if (pc_App != NULL)
         {
            pc_App->SetState(C_SyvUpUpdatePackageListNodeItemWidget::hu32_STATE_TO_DO);

            // Compare all applications
            for (uint32 u32_AppCounter = 0U; u32_AppCounter < u32_AppCountOnTarget; ++u32_AppCounter)
            {
               QString c_AppName = "";
               QString c_AppVersion = "";
               QString c_AppBuildTime = "";
               QString c_AppBuildDate = "";
               bool q_Valid = false;

               if (orc_DeviceInformation.pc_OSYDevice != NULL)
               {
                  const C_OSCProtocolDriverOsy::C_FlashBlockInfo & rc_FlashBlockInfo =
                     orc_DeviceInformation.pc_OSYDevice->c_Applications[u32_AppCounter];

                  c_AppName = rc_FlashBlockInfo.c_ApplicationName.c_str();
                  c_AppVersion = rc_FlashBlockInfo.c_ApplicationVersion.c_str();
                  c_AppBuildTime = rc_FlashBlockInfo.c_BuildTime.c_str();
                  c_AppBuildDate = rc_FlashBlockInfo.c_BuildDate.c_str();

                  q_Valid = (rc_FlashBlockInfo.u8_SignatureValid == 0U);
               }
               else
               {
                  //STW flashloader device
                  const stw_diag_lib::C_XFLECUInformation & rc_DeviceInfoBlock =
                     orc_DeviceInformation.pc_STWDevice->c_BasicInformation.c_DeviceInfoBlocks[u32_AppCounter];
                  c_AppName = rc_DeviceInfoBlock.acn_ProjectName;
                  c_AppVersion = rc_DeviceInfoBlock.acn_ProjectVersion;
                  c_AppBuildTime = rc_DeviceInfoBlock.acn_Time;
                  c_AppBuildDate = rc_DeviceInfoBlock.acn_Date;

                  // No application valid flag
                  q_Valid = true;
               }
               if ((q_Valid == true) &&
                   (pc_App->IsFileIdentical(c_AppName, c_AppVersion, c_AppBuildTime, c_AppBuildDate) == true))
               {
                  pc_App->SetState(C_SyvUpUpdatePackageListNodeItemWidget::hu32_STATE_FINISHED);
                  break;
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if mime valid

   Datablocks accept only one file for a specific datablock. No multiple files. No adding of new files.

   \param[in]   orc_PathList           File paths to analyze
   \param[in]   orc_Pos                Mouse position
   \param[out]  opc_RelevantFilePaths  File paths which could be used for this list
   \param[out]  oppc_App               Found application widget

   \return
   true  Valid
   false Invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPackageListNodeDatablocksWidget::CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos,
                                                       QStringList * const opc_RelevantFilePaths,
                                                       C_SyvUpUpdatePackageListNodeItemWidget ** const oppc_App) const
{
   bool q_Retval = false;

   if (orc_PathList.size() == 1)
   {
      C_SyvUpUpdatePackageListNodeItemWidget * pc_App = NULL;

      q_Retval = C_SyvUpUpdatePackageListNodeWidget::CheckMime(orc_PathList, orc_Pos, opc_RelevantFilePaths, &pc_App);

      // Dropping on datablocks must have a specific app as target
      if ((q_Retval == true) &&
          (pc_App != NULL))
      {
         if (oppc_App != NULL)
         {
            *oppc_App = pc_App;
         }

         if (opc_RelevantFilePaths != NULL)
         {
            opc_RelevantFilePaths->push_back(orc_PathList[0]);
            orc_PathList.erase(orc_PathList.begin());
         }
      }
      else
      {
         q_Retval = false;
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get type of subsection (data block/parameter set file/file based node file)

   \retval type data block
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpPackageListNodeDatablocksWidget::Type(void) const
{
   return mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of the applications in case of datablock configuration

   \param[in]  orc_Node          Current node
   \param[in]  orc_UpdateInfo    Configured update configuration of view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeDatablocksWidget::m_InitSpecificItem(const stw_opensyde_core::C_OSCNode & orc_Node,
                                                                const stw_opensyde_gui_logic::C_PuiSvNodeUpdate & orc_UpdateInfo)
{
   if (orc_Node.c_Applications.size() > 0)
   {
      const std::vector<QString> c_ViewAppPaths = orc_UpdateInfo.GetPaths(C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
      uint32 u32_Counter;

      tgl_assert(orc_Node.c_Applications.size() == c_ViewAppPaths.size());

      for (u32_Counter = 0U; u32_Counter < c_ViewAppPaths.size(); ++u32_Counter)
      {
         C_SyvUpPackageListNodeItemDatablockWidget * const pc_AppWidget =
            new C_SyvUpPackageListNodeItemDatablockWidget(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                          this->mc_DeviceType,
                                                          this->mq_FileBased, this->mq_StwFlashloader, this);
         const C_OSCNodeApplication * pc_App;

         pc_App = &orc_Node.c_Applications[u32_Counter];
         pc_AppWidget->SetAppName(pc_App->c_Name.c_str());

         // Check if a specific path is available
         if (c_ViewAppPaths[u32_Counter] != "")
         {
            pc_AppWidget->SetAppFile(c_ViewAppPaths[u32_Counter], false);
         }
         else
         {
            pc_AppWidget->SetAppFile(C_PuiUtil::h_MakeIndependentOfDbProjectPath(pc_App->c_ProjectPath.c_str(),
                                                                                 pc_App->c_ResultPath.c_str()), true);
         }

         // Set the default path for comparing with import configuration
         pc_AppWidget->SetDefaultFile(C_PuiUtil::h_MakeIndependentOfDbProjectPath(pc_App->c_ProjectPath.c_str(),
                                                                                  pc_App->c_ResultPath.c_str()));

         pc_AppWidget->SetAppType(pc_App->e_Type);
         pc_AppWidget->SetAppNumber(u32_Counter);

         ++this->mu32_FileCount;
         this->mpc_Ui->pc_AppVerticalLayout->addWidget(pc_AppWidget);
      }  //lint !e429  //no memory leak because of the parent of pc_AppWidget and the Qt memory management
   }

   this->mq_ShowAddButton = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeDatablocksWidget::m_InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabTitle->setText(C_GtGetText::h_GetText("Data Blocks"));
   this->mpc_Ui->pc_LabIcon->SetSvg(":images/system_views/IconFileDataBlock.svg");
}
