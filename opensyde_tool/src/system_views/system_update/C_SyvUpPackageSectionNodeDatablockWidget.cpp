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

#include "C_SyvUpPackageSectionNodeDatablockWidget.h"
#include "ui_C_SyvUpPackageSectionNodeWidget.h"
#include "C_OSCNodeApplication.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_SyvUpPackageListNodeItemDatablockWidget.h"
#include "C_SyvUpPackageListNodeItemParamSetWidget.h"
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
const QString C_SyvUpPackageSectionNodeDatablockWidget::mhc_RemovePathText("<Add File>");

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
C_SyvUpPackageSectionNodeDatablockWidget::C_SyvUpPackageSectionNodeDatablockWidget(QWidget * const opc_Parent) :
   C_SyvUpPackageSectionNodeWidget(opc_Parent),
   me_Type(stw_opensyde_core::C_OSCNodeApplication::eBINARY)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new file for the application

   \param[in]  orc_File    New path (already contains Db Project path and might be relative to openSYDE project)
   \param[in]  opc_App     Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeDatablockWidget::AdaptFile(const QString & orc_File,
                                                         C_SyvUpPackageListNodeItemWidget * const opc_App)
{
   bool q_FileIsOk = false;

   if (orc_File == C_GtGetText::h_GetText(mhc_RemovePathText.toStdString().c_str()))
   {
      // Special case. Removing file. No check necessary
      if (this->me_Type == C_OSCNodeApplication::ePARAMETER_SET_HALC)
      {
         // Special case: Datablock has psi files
         const C_PuiSvNodeUpdateParamInfo c_ParamFileInfo;
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

         // Nothing is to do anymore her -> Do not set q_FileIsOk to true!
      }
      else
      {
         q_FileIsOk = true;
      }
   }
   else
   {
      // Check the file
      // note: Data Block project path is already prefixed, so we only make absolute to openSYDE project
      const QString c_AbsoluteFilePath = C_PuiUtil::h_GetResolvedAbsPathFromProject(orc_File);
      const QFileInfo c_FileInfo(c_AbsoluteFilePath);
      const bool q_FileExists = c_FileInfo.exists() && c_FileInfo.isFile();

      if (q_FileExists == true)
      {
         if (this->me_Type == C_OSCNodeApplication::ePARAMETER_SET_HALC)
         {
            // Special case: Datablock has psi files
            this->m_AdaptParamSetFile(orc_File, opc_App);
            // Nothing is to do anymore her -> Do not set q_FileIsOk to true!
         }
         else if (this->mq_FileBased == false)
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
                           static_cast<QString>(C_GtGetText::h_GetText(
                                                   "Device type of %1 does not match node type %2."))
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
               const QString c_Details = static_cast<QString>(C_GtGetText::h_GetText("File path: %1")).arg(
                  c_AbsoluteFilePath);
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
      C_SyvUpPackageSectionNodeWidget::AdaptFile(orc_File, opc_App);

      tgl_assert(C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformationPath(this->mu32_ViewIndex,
                                                                               this->mu32_NodeIndex,
                                                                               this->mu32_SectionNumber,
                                                                               orc_File,
                                                                               C_PuiSvNodeUpdate::
                                                                               eFTP_DATA_BLOCK) == C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new file for the application

   \param[in]  oq_Skip     New skip flag
   \param[in]  opc_App     Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeDatablockWidget::SetSkipOfUpdateFile(const bool oq_Skip,
                                                                   C_SyvUpPackageListNodeItemWidget * const opc_App)
{
   if (opc_App != NULL)
   {
      C_SyvUpPackageSectionNodeWidget::SetSkipOfUpdateFile(oq_Skip, opc_App);

      if (opc_App->GetType() == mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK)
      {
         // In this case section number equals the path number
         tgl_assert(C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformationSkipUpdateOfPath(
                       this->mu32_ViewIndex,
                       this->mu32_NodeIndex,
                       this->mu32_SectionNumber, oq_Skip,
                       C_PuiSvNodeUpdate::eFTP_DATA_BLOCK) == C_NO_ERR);
      }
      else
      {
         // In this case only one NVM HALC Datablock can exist, so the app number equals the param set file number
         tgl_assert(C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformationSkipUpdateOfParamInfo(
                       this->mu32_ViewIndex,
                       this->mu32_NodeIndex,
                       opc_App->GetAppNumber(), oq_Skip) == C_NO_ERR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Revert the current application to the system definition default path

   Only for address based devices

   \param[in]  opc_App  Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeDatablockWidget::RevertFile(C_SyvUpPackageListNodeItemWidget * const opc_App)
{
   // Reverting path only available for datablock applications
   if (opc_App != NULL)
   {
      const uint32 u32_AppNumber = opc_App->GetAppNumber();
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      if (pc_Node != NULL)
      {
         // only revert if default application path is not empty
         tgl_assert(u32_AppNumber < pc_Node->c_Applications[this->mu32_SectionNumber].c_ResultPaths.size());
         if (pc_Node->c_Applications[this->mu32_SectionNumber].c_ResultPaths[u32_AppNumber] != "")
         {
            // Restore the default path
            if ((this->mu32_SectionNumber < pc_Node->c_Applications.size()))
            {
               const QString c_Path = C_PuiUtil::h_MakeIndependentOfDbProjectPath(
                  pc_Node->c_Applications[this->mu32_SectionNumber].c_ProjectPath.c_str(),
                  pc_Node->c_Applications[this->mu32_SectionNumber].c_ResultPaths[u32_AppNumber].c_str());
               if (this->me_Type != C_OSCNodeApplication::ePARAMETER_SET_HALC)
               {
                  // Remove the view specific path
                  tgl_assert(C_PuiSvHandler::h_GetInstance()->
                             SetNodeUpdateInformationPath(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                          this->mu32_SectionNumber, "",
                                                          C_PuiSvNodeUpdate::eFTP_DATA_BLOCK) == C_NO_ERR);
               }
               else
               {
                  const C_PuiSvNodeUpdateParamInfo c_ParamInfo;
                  // Remove the view specific path
                  tgl_assert(C_PuiSvHandler::h_GetInstance()->
                             SetNodeUpdateInformationParamInfo(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                               u32_AppNumber, c_ParamInfo) == C_NO_ERR);
               }
               opc_App->SetAppFile(c_Path, true);
            }
         }
         else
         {
            C_OgeWiCustomMessage c_Message(this);
            c_Message.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                             "No default output file declared in properties of "
                                                             "Data Block \"%1\". ")).arg(this->mc_SectionName));
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
void C_SyvUpPackageSectionNodeDatablockWidget::RemoveFile(C_SyvUpPackageListNodeItemWidget * const opc_App)
{
   this->AdaptFile(C_GtGetText::h_GetText(mhc_RemovePathText.toStdString().c_str()), opc_App);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepares the update package node configuration

   \param[out]  orc_NodeConfig   Node configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeDatablockWidget::PrepareExportConfig(C_SyvUpUpdatePackageConfigNode & orc_NodeConfig)
const
{
   if (this->mpc_Ui->pc_FileVerticalLayout->count() > 0)
   {
      sintn sn_Counter;

      orc_NodeConfig.c_AppConfigs.reserve(orc_NodeConfig.c_AppConfigs.size() + this->mu32_PrimaryFileCount);
      orc_NodeConfig.c_ParamSetConfigs.reserve(orc_NodeConfig.c_ParamSetConfigs.size() + this->mu32_ParamSetFileCount);

      for (sn_Counter = 0; sn_Counter < this->mpc_Ui->pc_FileVerticalLayout->count(); ++sn_Counter)
      {
         QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(sn_Counter);

         if (pc_Item != NULL)
         {
            C_SyvUpPackageListNodeItemWidget * const pc_App =
               dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_Item->widget());

            if (pc_App != NULL)
            {
               if (pc_App->GetType() != mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PARAMSET)
               {
                  C_SyvUpUpdatePackageConfigNodeApp c_AppConfig;
                  c_AppConfig.c_Name = this->mc_SectionName;
                  c_AppConfig.e_Type = this->me_Type;

                  pc_App->PrepareExportConfig(c_AppConfig);

                  orc_NodeConfig.c_AppConfigs.push_back(c_AppConfig);
               }
               else
               {
                  // Special case: PSI File
                  orc_NodeConfig.c_ParamSetConfigs.push_back(pc_App->GetAppFilePath());
               }
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
void C_SyvUpPackageSectionNodeDatablockWidget::LoadImportConfig(const C_SyvUpUpdatePackageConfig & orc_Config)
{
   uint32 u32_NodeConfigCounter;

   // Search the matching configuration
   for (u32_NodeConfigCounter = 0; u32_NodeConfigCounter < orc_Config.c_NodeConfigs.size(); ++u32_NodeConfigCounter)
   {
      const C_SyvUpUpdatePackageConfigNode & rc_NodeConfig = orc_Config.c_NodeConfigs[u32_NodeConfigCounter];

      if ((rc_NodeConfig.c_Name == this->mc_NodeName) &&
          (rc_NodeConfig.c_DeviceType == this->mc_DeviceType))
      {
         uint32 u32_AppConfigCounter;
         sintn sn_FileCounter;

         if (this->me_Type != C_OSCNodeApplication::ePARAMETER_SET_HALC)
         {
            // Search the matching configuration for the normal Datablocks
            for (u32_AppConfigCounter = 0; u32_AppConfigCounter < rc_NodeConfig.c_AppConfigs.size();
                 ++u32_AppConfigCounter)
            {
               const C_SyvUpUpdatePackageConfigNodeApp & rc_AppConfig =
                  rc_NodeConfig.c_AppConfigs[u32_AppConfigCounter];
               if ((rc_AppConfig.c_Name == this->mc_SectionName) &&
                   (rc_AppConfig.e_Type == this->me_Type) &&
                   (rc_AppConfig.e_Type != C_OSCNodeApplication::ePARAMETER_SET_HALC))
               {
                  // Matching configuration found. Load the configuration for the applications
                  for (sn_FileCounter = 0; sn_FileCounter < this->mpc_Ui->pc_FileVerticalLayout->count();
                       ++sn_FileCounter)
                  {
                     QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(sn_FileCounter);

                     if (pc_Item != NULL)
                     {
                        C_SyvUpPackageListNodeItemWidget * const pc_App =
                           dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_Item->widget());

                        if (pc_App != NULL)
                        {
                           // Update UI and update view data handling
                           pc_App->LoadImportConfig(rc_AppConfig);
                           break;
                        }
                     }
                  }
               }
            }
         }
         else
         {
            // Search for a special case NVM HALC Datablock when parameter set image files exists
            if ((rc_NodeConfig.c_ParamSetConfigs.size() > 0) &&
                (rc_NodeConfig.c_ParamSetConfigs.size() >= this->mu32_ParamSetFileCount))
            {
               for (sn_FileCounter = 0; sn_FileCounter < this->mpc_Ui->pc_FileVerticalLayout->count();
                    ++sn_FileCounter)
               {
                  QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(sn_FileCounter);

                  if (pc_Item != NULL)
                  {
                     C_SyvUpPackageListNodeItemWidget * const pc_App =
                        dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_Item->widget());

                     if ((pc_App != NULL) &&
                         (pc_App->GetType() == mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PARAMSET))
                     {
                        const QString c_Path = rc_NodeConfig.c_ParamSetConfigs[sn_FileCounter];

                        if (mh_IsFileParamSetFile(c_Path) == true)
                        {
                           this->m_AdaptParamSetFile(c_Path, pc_App);
                        }
                     }
                  }
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
void C_SyvUpPackageSectionNodeDatablockWidget::UpdateDeviceInformation(const C_SyvUpDeviceInfo & orc_DeviceInformation)
{
   uint32 u32_AppCountOnTarget;
   sintn sn_WidgetCounter;
   sintn sn_FileCounter = 0;
   sintn sn_FinishedFileCounter = 0;

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
   //go through all files in the UI
   for (sn_WidgetCounter = 0; sn_WidgetCounter < this->mpc_Ui->pc_FileVerticalLayout->count();
        ++sn_WidgetCounter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(sn_WidgetCounter);

      if (pc_Item != NULL)
      {
         C_SyvUpPackageListNodeItemWidget * const pc_File =
            dynamic_cast<C_SyvUpPackageListNodeItemWidget *>(pc_Item->widget());

         if (pc_File != NULL)
         {
            ++sn_FileCounter;
            pc_File->SetState(C_SyvUpPackageListNodeItemWidget::hu32_STATE_TO_DO);

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
                   (pc_File->IsFileIdentical(c_AppName, c_AppVersion, c_AppBuildTime, c_AppBuildDate) == true))
               {
                  ++sn_FinishedFileCounter;
                  pc_File->SetState(C_SyvUpPackageListNodeItemWidget::hu32_STATE_FINISHED);
                  break;
               }
            }
         }
      }
   }

   if (sn_FileCounter == sn_FinishedFileCounter)
   {
      // All files of Datablock finished
      this->m_SetState(C_SyvUpPackageListNodeItemWidget::hu32_STATE_FINISHED);
   }
   else
   {
      this->m_SetState(C_SyvUpPackageListNodeItemWidget::hu32_STATE_TO_DO);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if mime valid

   Datablocks accept only one file for a specific datablock. No multiple files. No adding of new files.

   \param[in]   orc_PathList                    File paths to analyze
   \param[in]   orc_Pos                         Mouse position
   \param[out]  opc_RelevantFilePaths           File paths which could be used for this list
   \param[out]  opc_RelevantParamSetImagePaths  Parameter set image paths which could be used for this list
   \param[out]  oppc_App                        Found application widget

   \return
   true  Valid
   false Invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPackageSectionNodeDatablockWidget::CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos,
                                                         QStringList * const opc_RelevantFilePaths,
                                                         QStringList * const opc_RelevantParamSetImagePaths,
                                                         C_SyvUpPackageListNodeItemWidget ** const oppc_App) const
{
   bool q_Retval = false;

   if (orc_PathList.size() == 1)
   {
      C_SyvUpPackageListNodeItemWidget * pc_App = NULL;

      q_Retval = C_SyvUpPackageSectionNodeWidget::CheckMime(orc_PathList, orc_Pos, opc_RelevantFilePaths,
                                                            opc_RelevantParamSetImagePaths, &pc_App);

      // Dropping on datablocks must have a specific app as target
      if ((q_Retval == true) &&
          (pc_App != NULL))
      {
         if (this->me_Type != C_OSCNodeApplication::ePARAMETER_SET_HALC)
         {
            if (opc_RelevantFilePaths != NULL)
            {
               opc_RelevantFilePaths->push_back(orc_PathList[0]);
               orc_PathList.erase(orc_PathList.begin());
            }
         }
         else
         {
            if (mh_IsFileParamSetFile(orc_PathList[0]) == true)
            {
               if (opc_RelevantParamSetImagePaths != NULL)
               {
                  opc_RelevantParamSetImagePaths->push_back(orc_PathList[0]);
                  orc_PathList.erase(orc_PathList.begin());
               }
            }
            else
            {
               // Not the correct file format, in this case a psi file is expected
               q_Retval = false;
            }
         }

         if ((q_Retval == true) &&
             (oppc_App != NULL))
         {
            *oppc_App = pc_App;
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
uint32 C_SyvUpPackageSectionNodeDatablockWidget::Type(void) const
{
   return mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of the applications in case of datablock configuration

   \param[in]  orc_Node          Current node
   \param[in]  orc_UpdateInfo    Configured update configuration of view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeDatablockWidget::m_InitSpecificItem(const stw_opensyde_core::C_OSCNode & orc_Node,
                                                                  const stw_opensyde_gui_logic::C_PuiSvNodeUpdate & orc_UpdateInfo)
{
   // mu32_SectionNumber equals in case of datablock widget the index of the datablock
   tgl_assert(this->mu32_SectionNumber < orc_Node.c_Applications.size());
   if (this->mu32_SectionNumber < orc_Node.c_Applications.size())
   {
      const C_OSCNodeApplication & rc_Datablock = orc_Node.c_Applications[this->mu32_SectionNumber];

      if (rc_Datablock.e_Type != C_OSCNodeApplication::ePARAMETER_SET_HALC)
      {
         std::vector<QString> c_ViewDatablockPaths = orc_UpdateInfo.GetPaths(C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
         std::vector<bool> c_ViewDatablockSkipFlags = orc_UpdateInfo.GetSkipUpdateOfPathsFlags(
            C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
         C_SyvUpPackageListNodeItemDatablockWidget * const pc_FileWidget =
            new C_SyvUpPackageListNodeItemDatablockWidget(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                          this->mc_DeviceType,
                                                          this->mq_FileBased, this->mq_StwFlashloader, this);

         tgl_assert(rc_Datablock.c_ResultPaths.size() == 1);
         tgl_assert(this->mu32_SectionNumber < c_ViewDatablockPaths.size());

         this->mc_SectionName = rc_Datablock.c_Name.c_str();
         this->me_Type = rc_Datablock.e_Type;

         if (c_ViewDatablockSkipFlags.size() != c_ViewDatablockPaths.size())
         {
            c_ViewDatablockSkipFlags.resize(c_ViewDatablockPaths.size(), false);
         }

         // Must be set before the current path
         // Set the default path for comparing with import configuration
         pc_FileWidget->SetDefaultFile(C_PuiUtil::h_MakeIndependentOfDbProjectPath(
                                          rc_Datablock.c_ProjectPath.c_str(),
                                          rc_Datablock.c_ResultPaths[0].c_str()));

         // Check if a specific path is available
         if (c_ViewDatablockPaths[this->mu32_SectionNumber] != "")
         {
            pc_FileWidget->SetAppFile(c_ViewDatablockPaths[this->mu32_SectionNumber], false);
         }
         else
         {
            pc_FileWidget->SetAppFile(C_PuiUtil::h_MakeIndependentOfDbProjectPath(
                                         rc_Datablock.c_ProjectPath.c_str(),
                                         rc_Datablock.c_ResultPaths[0].c_str()),
                                      true);
         }
         pc_FileWidget->SetSkipOfUpdateFile(c_ViewDatablockSkipFlags[this->mu32_SectionNumber]);

         pc_FileWidget->SetAppNumber(0U);
         pc_FileWidget->SetOwnerSectionName(this->mc_SectionName);

         ++this->mu32_FileCount;
         ++this->mu32_PrimaryFileCount;
         this->mpc_Ui->pc_FileVerticalLayout->addWidget(pc_FileWidget);
      } //lint !e429  //no memory leak because of the parent of pc_FileWidget and the Qt memory management
      else
      {
         // Special case: Datablock for NVM HALC parameter set image files
         const std::vector<C_PuiSvNodeUpdateParamInfo> c_ViewParamsetPaths = orc_UpdateInfo.GetParamInfos();
         std::vector<bool> c_ViewParamSetSkipFlags = orc_UpdateInfo.GetSkipUpdateOfParamInfosFlags();
         uint32 u32_ParamSetFileCounter;

         for (u32_ParamSetFileCounter = 0U; u32_ParamSetFileCounter < rc_Datablock.c_ResultPaths.size();
              ++u32_ParamSetFileCounter)
         {
            C_PuiSvNodeUpdateParamInfo c_ParamInfo;
            C_SyvUpPackageListNodeItemParamSetWidget * const pc_ParamWidget =
               new C_SyvUpPackageListNodeItemParamSetWidget(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                            this->mc_DeviceType,
                                                            this->mq_FileBased, this->mq_StwFlashloader, this);
            const QString c_DefaultPath = C_PuiUtil::h_MakeIndependentOfDbProjectPath(
               rc_Datablock.c_ProjectPath.c_str(),
               rc_Datablock.c_ResultPaths[u32_ParamSetFileCounter].c_str());
            C_PuiSvNodeUpdateParamInfo c_FileInfo;

            this->mc_SectionName = rc_Datablock.c_Name.c_str();
            this->me_Type = rc_Datablock.e_Type;

            if (c_ViewParamSetSkipFlags.size() != c_ViewParamsetPaths.size())
            {
               c_ViewParamSetSkipFlags.resize(c_ViewParamsetPaths.size(), false);
            }

            tgl_assert(u32_ParamSetFileCounter < c_ViewParamsetPaths.size());
            if (u32_ParamSetFileCounter < c_ViewParamsetPaths.size())
            {
               // Get the view specific configuration
               c_FileInfo = c_ViewParamsetPaths[u32_ParamSetFileCounter];
            }

            // Must be set before the current path
            pc_ParamWidget->SetDefaultFile(c_DefaultPath);

            // Check if a specific path is available
            if (c_FileInfo.GetPath() != "")
            {
               // System view specific path
               pc_ParamWidget->SetAppFile(c_FileInfo.GetPath(), false);
               //Set initial param set info
               c_ParamInfo.SetContent(c_FileInfo.GetPath(), c_FileInfo.GetLastKnownCrc());
            }
            else
            {
               // System definition default path
               pc_ParamWidget->SetAppFile(c_DefaultPath, true);
               //Set initial param set info
               c_ParamInfo.SetContent(c_DefaultPath, c_FileInfo.GetLastKnownCrc());
            }
            pc_ParamWidget->SetSkipOfUpdateFile(c_ViewParamSetSkipFlags[u32_ParamSetFileCounter]);

            pc_ParamWidget->SetAppNumber(u32_ParamSetFileCounter);
            pc_ParamWidget->SetParamInfo(c_ParamInfo);
            pc_ParamWidget->SetOwnerSectionName(this->mc_SectionName);

            ++this->mu32_FileCount;
            ++this->mu32_ParamSetFileCount;
            this->mpc_Ui->pc_FileVerticalLayout->addWidget(pc_ParamWidget);
         } //lint !e429  //no memory leak because of the parent of pc_ParamWidget and the Qt memory management
      }
   }

   this->m_UpdateTitle();
   this->mq_ShowAddButton = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_SyvUpPackageSectionNodeDatablockWidget::m_InitStaticNames(void)
{
   // Nothing to do
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update of the title label
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageSectionNodeDatablockWidget::m_UpdateTitle(void)
{
   const QString c_Title = "%1 #%2 - %3";

   this->mc_Title = c_Title.arg(static_cast<QString>(C_GtGetText::h_GetText("Datablock")),
                                QString::number(this->mu32_SectionNumber + 1U),
                                this->mc_SectionName);

   C_SyvUpPackageSectionNodeWidget::m_UpdateTitle();
}
