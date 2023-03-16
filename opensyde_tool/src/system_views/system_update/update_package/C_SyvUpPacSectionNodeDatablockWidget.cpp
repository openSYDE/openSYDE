//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for listing and handling datablock files

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QFileInfo>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_Uti.hpp"
#include "C_PuiUtil.hpp"

#include "C_SyvUpPacSectionNodeDatablockWidget.hpp"
#include "ui_C_SyvUpPacSectionNodeWidget.h"
#include "C_OscNodeApplication.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_SyvUpPacListNodeItemDatablockWidget.hpp"
#include "C_SyvUpPacListNodeItemParamSetWidget.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_OsyHexFile.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::scl;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvUpPacSectionNodeDatablockWidget::mhc_REMOVE_PATH_TEXT("<Add File>");

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
C_SyvUpPacSectionNodeDatablockWidget::C_SyvUpPacSectionNodeDatablockWidget(QWidget * const opc_Parent) :
   C_SyvUpPacSectionNodeWidget(opc_Parent),
   me_Type(stw::opensyde_core::C_OscNodeApplication::eBINARY)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new file for the application

   \param[in]  orc_File    New path (already contains Db Project path and might be relative to openSYDE project)
   \param[in]  opc_App     Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSectionNodeDatablockWidget::AdaptFile(const QString & orc_File,
                                                     C_SyvUpPacListNodeItemWidget * const opc_App)
{
   bool q_FileIsOk = false;

   if (orc_File == C_GtGetText::h_GetText(mhc_REMOVE_PATH_TEXT.toStdString().c_str()))
   {
      // Special case. Removing file. No check necessary
      if (this->me_Type == C_OscNodeApplication::ePARAMETER_SET_HALC)
      {
         // Special case: Datablock has psi files
         const C_OscViewNodeUpdateParamInfo c_ParamFileInfo;
         C_SyvUpPacListNodeItemParamSetWidget * const pc_ParamSetWidget =
            dynamic_cast<C_SyvUpPacListNodeItemParamSetWidget *>(opc_App);

         C_SyvUpPacSectionNodeWidget::AdaptFile(orc_File, opc_App);

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
         if (this->me_Type == C_OscNodeApplication::ePARAMETER_SET_HALC)
         {
            // Special case: Datablock has psi files
            this->m_AdaptParamSetFile(orc_File, opc_App);
            // Nothing is to do anymore her -> Do not set q_FileIsOk to true!
         }
         else if (this->mq_FileBased == false)
         {
            C_OsyHexFile * const pc_HexFile = new C_OsyHexFile();
            if (pc_HexFile->LoadFromFile(c_AbsoluteFilePath.toStdString().c_str()) == stw::hex_file::NO_ERR)
            {
               stw::diag_lib::C_XFLECUInformation c_FileApplicationInfo;
               const int32_t s32_Result = pc_HexFile->ScanApplicationInformationBlockFromHexFile(c_FileApplicationInfo);

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
                     const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
                        this->mu32_NodeIndex);
                     if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL) &&
                         (pc_Node->u32_SubDeviceIndex < pc_Node->pc_DeviceDefinition->c_SubDevices.size()))
                     {
                        for (uint32_t u32_ItName = 0UL;
                             u32_ItName <
                             pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].
                             c_OtherAcceptedNames.
                             size();
                             ++u32_ItName)
                        {
                           const QString c_AllowedDevice =
                              pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].
                              c_OtherAcceptedNames[u32_ItName].Trim().UpperCase().c_str();
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
      C_SyvUpPacSectionNodeWidget::AdaptFile(orc_File, opc_App);

      tgl_assert(C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformationPath(this->mu32_ViewIndex,
                                                                               this->mu32_NodeIndex,
                                                                               this->mu32_DataBlockPathNumber,
                                                                               orc_File,
                                                                               C_OscViewNodeUpdate::
                                                                               eFTP_DATA_BLOCK) == C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new file for the application

   \param[in]  oq_Skip  New skip flag
   \param[in]  opc_App  Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSectionNodeDatablockWidget::SetSkipOfUpdateFile(const bool oq_Skip,
                                                               C_SyvUpPacListNodeItemWidget * const opc_App)
{
   if (opc_App != NULL)
   {
      C_SyvUpPacSectionNodeWidget::SetSkipOfUpdateFile(oq_Skip, opc_App);

      if (opc_App->GetType() == mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK)
      {
         // In this case section number equals the path number
         tgl_assert(C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformationSkipUpdateOfPath(
                       this->mu32_ViewIndex,
                       this->mu32_NodeIndex,
                       this->mu32_DataBlockPathNumber, oq_Skip,
                       C_OscViewNodeUpdate::eFTP_DATA_BLOCK) == C_NO_ERR);
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
void C_SyvUpPacSectionNodeDatablockWidget::RevertFile(C_SyvUpPacListNodeItemWidget * const opc_App)
{
   // Reverting path only available for datablock applications
   if (opc_App != NULL)
   {
      const uint32_t u32_AppNumber = opc_App->GetAppNumber();
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
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
               if (this->me_Type != C_OscNodeApplication::ePARAMETER_SET_HALC)
               {
                  // Remove the view specific path
                  tgl_assert(C_PuiSvHandler::h_GetInstance()->
                             SetNodeUpdateInformationPath(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                          this->mu32_DataBlockPathNumber, "",
                                                          C_OscViewNodeUpdate::eFTP_DATA_BLOCK) == C_NO_ERR);
               }
               else
               {
                  const C_OscViewNodeUpdateParamInfo c_ParamInfo;
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
void C_SyvUpPacSectionNodeDatablockWidget::RemoveFile(C_SyvUpPacListNodeItemWidget * const opc_App)
{
   this->AdaptFile(C_GtGetText::h_GetText(mhc_REMOVE_PATH_TEXT.toStdString().c_str()), opc_App);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepares the update package node configuration

   \param[out]  orc_NodeConfig   Node configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSectionNodeDatablockWidget::PrepareExportConfig(C_SyvUpPacConfigNode & orc_NodeConfig)
const
{
   if (this->mpc_Ui->pc_FileVerticalLayout->count() > 0)
   {
      int32_t s32_Counter;

      orc_NodeConfig.c_AppConfigs.reserve(orc_NodeConfig.c_AppConfigs.size() + this->mu32_PrimaryFileCount);
      orc_NodeConfig.c_ParamSetConfigs.reserve(orc_NodeConfig.c_ParamSetConfigs.size() + this->mu32_ParamSetFileCount);

      for (s32_Counter = 0; s32_Counter < this->mpc_Ui->pc_FileVerticalLayout->count(); ++s32_Counter)
      {
         QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(s32_Counter);

         if (pc_Item != NULL)
         {
            C_SyvUpPacListNodeItemWidget * const pc_App =
               dynamic_cast<C_SyvUpPacListNodeItemWidget *>(pc_Item->widget());

            if (pc_App != NULL)
            {
               if (pc_App->GetType() != mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PARAMSET)
               {
                  C_SyvUpPacConfigNodeApp c_AppConfig;
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
void C_SyvUpPacSectionNodeDatablockWidget::LoadImportConfig(const C_SyvUpPacConfig & orc_Config)
{
   uint32_t u32_NodeConfigCounter;

   // Search the matching configuration
   for (u32_NodeConfigCounter = 0; u32_NodeConfigCounter < orc_Config.c_NodeConfigs.size(); ++u32_NodeConfigCounter)
   {
      const C_SyvUpPacConfigNode & rc_NodeConfig = orc_Config.c_NodeConfigs[u32_NodeConfigCounter];

      if ((rc_NodeConfig.c_Name == this->mc_NodeName) &&
          (rc_NodeConfig.c_DeviceType == this->mc_DeviceType))
      {
         int32_t s32_FileCounter;

         if (this->me_Type != C_OscNodeApplication::ePARAMETER_SET_HALC)
         {
            uint32_t u32_AppConfigCounter;
            // Search the matching configuration for the normal Datablocks
            for (u32_AppConfigCounter = 0; u32_AppConfigCounter < rc_NodeConfig.c_AppConfigs.size();
                 ++u32_AppConfigCounter)
            {
               const C_SyvUpPacConfigNodeApp & rc_AppConfig =
                  rc_NodeConfig.c_AppConfigs[u32_AppConfigCounter];
               if ((rc_AppConfig.c_Name == this->mc_SectionName) &&
                   (rc_AppConfig.e_Type == this->me_Type) &&
                   (rc_AppConfig.e_Type != C_OscNodeApplication::ePARAMETER_SET_HALC))
               {
                  // Matching configuration found. Load the configuration for the applications
                  for (s32_FileCounter = 0; s32_FileCounter < this->mpc_Ui->pc_FileVerticalLayout->count();
                       ++s32_FileCounter)
                  {
                     QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(s32_FileCounter);

                     if (pc_Item != NULL)
                     {
                        C_SyvUpPacListNodeItemWidget * const pc_App =
                           dynamic_cast<C_SyvUpPacListNodeItemWidget *>(pc_Item->widget());

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
               for (s32_FileCounter = 0; s32_FileCounter < this->mpc_Ui->pc_FileVerticalLayout->count();
                    ++s32_FileCounter)
               {
                  QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(s32_FileCounter);

                  if (pc_Item != NULL)
                  {
                     C_SyvUpPacListNodeItemWidget * const pc_App =
                        dynamic_cast<C_SyvUpPacListNodeItemWidget *>(pc_Item->widget());

                     if ((pc_App != NULL) &&
                         (pc_App->GetType() == mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PARAMSET))
                     {
                        const QString c_Path = rc_NodeConfig.c_ParamSetConfigs[s32_FileCounter];

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
void C_SyvUpPacSectionNodeDatablockWidget::UpdateDeviceInformation(const C_SyvUpDeviceInfo & orc_DeviceInformation)
{
   uint32_t u32_AppCountOnTarget;
   int32_t s32_WidgetCounter;
   int32_t s32_FileCounter = 0;
   int32_t s32_FinishedFileCounter = 0;

   //get number of applications present on target
   if (orc_DeviceInformation.pc_OpenSydeDevice != NULL)
   {
      u32_AppCountOnTarget = orc_DeviceInformation.pc_OpenSydeDevice->c_Applications.size();
   }
   else if (orc_DeviceInformation.pc_StwDevice != NULL)
   {
      u32_AppCountOnTarget = orc_DeviceInformation.pc_StwDevice->c_BasicInformation.c_DeviceInfoBlocks.GetLength();
   }
   else
   {
      // Nothing to do
      u32_AppCountOnTarget = 0U;
   }
   //go through all files in the UI
   for (s32_WidgetCounter = 0; s32_WidgetCounter < this->mpc_Ui->pc_FileVerticalLayout->count();
        ++s32_WidgetCounter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_FileVerticalLayout->itemAt(s32_WidgetCounter);

      if (pc_Item != NULL)
      {
         C_SyvUpPacListNodeItemWidget * const pc_File =
            dynamic_cast<C_SyvUpPacListNodeItemWidget *>(pc_Item->widget());

         if (pc_File != NULL)
         {
            ++s32_FileCounter;
            pc_File->SetState(C_SyvUpPacListNodeItemWidget::hu32_STATE_TO_DO);

            // Compare all applications
            for (uint32_t u32_AppCounter = 0U; u32_AppCounter < u32_AppCountOnTarget; ++u32_AppCounter)
            {
               QString c_AppName = "";
               QString c_AppVersion = "";
               QString c_AppBuildTime = "";
               QString c_AppBuildDate = "";
               bool q_Valid = false;

               if (orc_DeviceInformation.pc_OpenSydeDevice != NULL)
               {
                  const C_OscProtocolDriverOsy::C_FlashBlockInfo & rc_FlashBlockInfo =
                     orc_DeviceInformation.pc_OpenSydeDevice->c_Applications[u32_AppCounter];

                  c_AppName = rc_FlashBlockInfo.c_ApplicationName.c_str();
                  c_AppVersion = rc_FlashBlockInfo.c_ApplicationVersion.c_str();
                  c_AppBuildTime = rc_FlashBlockInfo.c_BuildTime.c_str();
                  c_AppBuildDate = rc_FlashBlockInfo.c_BuildDate.c_str();

                  q_Valid = (rc_FlashBlockInfo.u8_SignatureValid == 0U);
               }
               else
               {
                  //STW flashloader device
                  const stw::diag_lib::C_XFLECUInformation & rc_DeviceInfoBlock =
                     orc_DeviceInformation.pc_StwDevice->c_BasicInformation.c_DeviceInfoBlocks[u32_AppCounter];
                  c_AppName = rc_DeviceInfoBlock.GetProjectName().c_str();
                  c_AppVersion = rc_DeviceInfoBlock.GetProjectVersion().c_str();
                  c_AppBuildTime = rc_DeviceInfoBlock.GetTime().c_str();
                  c_AppBuildDate = rc_DeviceInfoBlock.GetDate().c_str();

                  // No application valid flag
                  q_Valid = true;
               }
               if ((q_Valid == true) &&
                   (pc_File->IsFileIdentical(c_AppName, c_AppVersion, c_AppBuildTime, c_AppBuildDate) == true))
               {
                  ++s32_FinishedFileCounter;
                  pc_File->SetState(C_SyvUpPacListNodeItemWidget::hu32_STATE_FINISHED);
                  break;
               }
            }
         }
      }
   }

   if (s32_FileCounter == s32_FinishedFileCounter)
   {
      // All files of Datablock finished
      this->m_SetState(C_SyvUpPacListNodeItemWidget::hu32_STATE_FINISHED);
   }
   else
   {
      this->m_SetState(C_SyvUpPacListNodeItemWidget::hu32_STATE_TO_DO);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if mime valid

   Datablocks accept only one file for a specific datablock. No multiple files. No adding of new files.

   \param[in]   orc_PathList                    File paths to analyze
   \param[in]   orc_Pos                         Mouse position
   \param[out]  opc_RelevantFilePaths           File paths which could be used for this list
   \param[out]  opc_RelevantParamSetImagePaths  Parameter set image paths which could be used for this list
   \param[out]  opc_RelevantPemFilePaths        PEM file paths (Not relevant and possible for datablocks)
   \param[out]  oppc_App                        Found application widget

   \return
   true  Valid
   false Invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacSectionNodeDatablockWidget::CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos,
                                                     QStringList * const opc_RelevantFilePaths,
                                                     QStringList * const opc_RelevantParamSetImagePaths,
                                                     QStringList * const opc_RelevantPemFilePaths,
                                                     C_SyvUpPacListNodeItemWidget ** const oppc_App) const
{
   bool q_Retval = false;

   if (orc_PathList.size() == 1)
   {
      C_SyvUpPacListNodeItemWidget * pc_App = NULL;

      q_Retval = C_SyvUpPacSectionNodeWidget::CheckMime(orc_PathList, orc_Pos, opc_RelevantFilePaths,
                                                        opc_RelevantParamSetImagePaths,
                                                        opc_RelevantPemFilePaths, &pc_App);

      // Dropping on datablocks must have a specific app as target
      if ((q_Retval == true) &&
          (pc_App != NULL))
      {
         if (this->me_Type != C_OscNodeApplication::ePARAMETER_SET_HALC)
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
uint32_t C_SyvUpPacSectionNodeDatablockWidget::Type(void) const
{
   return mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of the applications in case of datablock configuration

   \param[in]  orc_Node          Current node
   \param[in]  orc_UpdateInfo    Configured update configuration of view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSectionNodeDatablockWidget::m_InitSpecificItem(const stw::opensyde_core::C_OscNode & orc_Node,
                                                              const stw::opensyde_core::C_OscViewNodeUpdate & orc_UpdateInfo)
{
   // mu32_SectionNumber equals in case of datablock widget the index of the datablock
   tgl_assert(this->mu32_SectionNumber < orc_Node.c_Applications.size());

   if (this->mu32_SectionNumber < orc_Node.c_Applications.size())
   {
      const C_OscNodeApplication & rc_Datablock = orc_Node.c_Applications[this->mu32_SectionNumber];

      if (rc_Datablock.e_Type != C_OscNodeApplication::ePARAMETER_SET_HALC)
      {
         std::vector<C_SclString> c_ViewDatablockPaths = orc_UpdateInfo.GetPaths(C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
         std::vector<bool> c_ViewDatablockSkipFlags = orc_UpdateInfo.GetSkipUpdateOfPathsFlags(
            C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
         C_SyvUpPacListNodeItemDatablockWidget * const pc_FileWidget =
            new C_SyvUpPacListNodeItemDatablockWidget(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                      this->mc_DeviceType,
                                                      this->mq_FileBased, this->mq_StwFlashloader, this);

         tgl_assert(rc_Datablock.c_ResultPaths.size() == 1);
         tgl_assert(this->mu32_DataBlockPathNumber < c_ViewDatablockPaths.size());

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
         if (c_ViewDatablockPaths[this->mu32_DataBlockPathNumber] != "")
         {
            pc_FileWidget->SetAppFile(c_ViewDatablockPaths[this->mu32_DataBlockPathNumber].c_str(), false);
         }
         else
         {
            pc_FileWidget->SetAppFile(C_PuiUtil::h_MakeIndependentOfDbProjectPath(
                                         rc_Datablock.c_ProjectPath.c_str(),
                                         rc_Datablock.c_ResultPaths[0].c_str()),
                                      true);
         }
         pc_FileWidget->SetSkipOfUpdateFile(c_ViewDatablockSkipFlags[this->mu32_DataBlockPathNumber]);

         pc_FileWidget->SetAppNumber(0U);
         pc_FileWidget->SetOwnerSectionName(this->mc_SectionName);

         ++this->mu32_FileCount;
         ++this->mu32_PrimaryFileCount;
         this->mpc_Ui->pc_FileVerticalLayout->addWidget(pc_FileWidget);
      } //lint !e429  //no memory leak because of the parent of pc_FileWidget and the Qt memory management
      else
      {
         // Special case: Datablock for NVM HALC parameter set image files
         const std::vector<C_OscViewNodeUpdateParamInfo> c_ViewParamsetPaths = orc_UpdateInfo.GetParamInfos();
         std::vector<bool> c_ViewParamSetSkipFlags = orc_UpdateInfo.GetSkipUpdateOfParamInfosFlags();
         uint32_t u32_ParamSetFileCounter;

         for (u32_ParamSetFileCounter = 0U; u32_ParamSetFileCounter < rc_Datablock.c_ResultPaths.size();
              ++u32_ParamSetFileCounter)
         {
            C_OscViewNodeUpdateParamInfo c_ParamInfo;
            C_SyvUpPacListNodeItemParamSetWidget * const pc_ParamWidget =
               new C_SyvUpPacListNodeItemParamSetWidget(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                        this->mc_DeviceType,
                                                        this->mq_FileBased, this->mq_StwFlashloader, this);
            const QString c_DefaultPath = C_PuiUtil::h_MakeIndependentOfDbProjectPath(
               rc_Datablock.c_ProjectPath.c_str(),
               rc_Datablock.c_ResultPaths[u32_ParamSetFileCounter].c_str());
            C_OscViewNodeUpdateParamInfo c_FileInfo;

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
               pc_ParamWidget->SetAppFile(c_FileInfo.GetPath().c_str(), false);
               //Set initial param set info
               c_ParamInfo.SetContent(c_FileInfo.GetPath(), c_FileInfo.GetLastKnownCrc());
            }
            else
            {
               // System definition default path
               pc_ParamWidget->SetAppFile(c_DefaultPath, true);
               //Set initial param set info
               c_ParamInfo.SetContent(c_DefaultPath.toStdString().c_str(), c_FileInfo.GetLastKnownCrc());
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
void C_SyvUpPacSectionNodeDatablockWidget::m_InitStaticNames(void)
{
   // Nothing to do
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update of the title label
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSectionNodeDatablockWidget::m_UpdateTitle(void)
{
   const QString c_Title = "%1 #%2 - %3";

   this->mc_Title = c_Title.arg(static_cast<QString>(C_GtGetText::h_GetText("Data Block")),
                                QString::number(this->mu32_SectionNumber + 1U),
                                this->mc_SectionName);

   C_SyvUpPacSectionNodeWidget::m_UpdateTitle();
}
